/*
 * Copyright (C) 2013-2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/
#include <assert.h>

#include <QByteArray>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>

#include "clickupdatechecker.h"
#include "helpers.h"


namespace UpdatePlugin {

ClickUpdateChecker::ClickUpdateChecker(QObject *parent):
    ClickApiProto(parent),
    m_cache(QStandardPaths::writableLocation(
        QStandardPaths::CacheLocation)
        + "/click-metadata-cache.json")
{
    readFromCache();

    initializeProcess();
}

ClickUpdateChecker::~ClickUpdateChecker()
{
}

void ClickUpdateChecker::readFromCache()
{
    if (!m_cache.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "could not open cache file," << m_cache.errorString();
    } else {
        QString cachedJson = m_cache.readAll();
        QJsonDocument d = QJsonDocument::fromJson(cachedJson.toUtf8());
        QJsonObject o = d.object();

        // How old is this cache?
        int cachedAt = o.value(QStringLiteral("cached_at")).toInt(0);
        uint now = QDateTime::currentDateTime().toTime_t();
        // If the cache timestamp is older than 24 hours, don't use it.
        if (cachedAt < (now - 86400)) {

        }
        qWarning() << cachedAt << now;

    }
    m_cache.close();

}

void ClickUpdateChecker::initializeMeta(const QSharedPointer<ClickUpdateMetadata> &meta)
{
    QObject::connect(meta.data(), SIGNAL(credentialError()),
                     this, SIGNAL(credentialError()));
    QObject::connect(meta.data(), SIGNAL(clickTokenRequestSucceeded(const ClickUpdateMetadata*)),
                     this, SLOT(handleMetadataClickTokenObtained(const ClickUpdateMetadata*)));
    QObject::connect(meta.data(), SIGNAL(clickTokenRequestFailed()),
                     this, SLOT(handleClickTokenRequestFailed()));
}

void ClickUpdateChecker::initializeProcess()
{
    QObject::connect(&m_process, SIGNAL(finished(const int&)),
                     this, SLOT(handleInstalledClicks(const int&)));
}

// This method is quite complex, and naturally not synchronous. Basically,
// there are three async operations, with one for each
// updated click (fetching a click token). Each of these can fail in various
// ways, so we report either serverError or networkError with an accompanying
// errorString.
//
// The first operation is obtaining the list of clicks installed on the system.
// The second is receiving the metadata for clicks we found installed.
// The third is signing the download url for each click update. We ask the
// ClickUpdateMetadata to do this for us.
//
// All of this can be bypassed if we find cached metadata newer than 24 hours.
void ClickUpdateChecker::check()
{
    qWarning() << "click checker: check...";
    // No token, so don't check anything, but report that the check
    // completed. We should not be given an invalid token here,
    // and we've yet to talk to the server so we can't know
    // the state of the token.
    if (!m_token.isValid()) {
        Q_EMIT checkCompleted();
        return;
    }

    // If we already have metas, we assume they can
    if (m_metas.count() > 0) {
        Q_EMIT checkCompleted();
        return;
    }

    setErrorString("");

    // Start process of getting the list of installed clicks.
    QStringList args("list");
    args << "--manifest";
    QString command = Helpers::whichClick();
    m_process.start(command, args);
    if (!m_process.waitForStarted()) {
        handleProcessError(m_process.error());
    }
}

// Tries to shut down all checks we might currently be doing.
void ClickUpdateChecker::cancel()
{
    // Abort all click update metadata objects.
    foreach (const QString &name, m_metas.keys())
        m_metas.value(name)->cancel();

    m_process.terminate();

    // This tells active replies to abort.
    Q_EMIT abortNetworking();

    Q_EMIT checkCompleted();
}

void ClickUpdateChecker::handleInstalledClicks(const int &exitCode)
{
    qWarning() << "click checker: handleInstalledClicks..." << exitCode;
    if (exitCode > 0) {
        setErrorString(
            QString("list command exited with code %1.").arg(exitCode)
        );
        Q_EMIT checkCompleted();
        return;
    }

    QString output(m_process.readAllStandardOutput());
    QJsonDocument document = QJsonDocument::fromJson(output.toUtf8());
    QJsonArray array = document.array();

    // Nothing to do.
    if (array.size() == 0) {
        qWarning() << "click checker: nothing to do, no clicks installed.";
        Q_EMIT checkCompleted();
        return;
    }

    int i;
    for (i = 0; i < array.size(); i++) {
        QJsonObject object = array.at(i).toObject();

        QSharedPointer<ClickUpdateMetadata> meta(new ClickUpdateMetadata);

        initializeMeta(meta);

        meta->setToken(m_token);
        meta->setName(object.value("name").toString());
        meta->setTitle(object.value("title").toString());
        meta->setLocalVersion(object.value("version").toString());
        m_metas.insert(meta->name(), meta);
        qWarning() << "click checker: queueing up" << meta->name();
    }

    // Populate each ClickUpdateMetadata with remote metadata.
    requestClickMetadata();
}

void ClickUpdateChecker::handleProcessError(const QProcess::ProcessError &error)
{
    qWarning() << "click checker: process failed";
// QProcess::FailedToStart 0   The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.
// QProcess::Crashed   1   The process crashed some time after starting successfully.
// QProcess::Timedout  2   The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.
// QProcess::WriteError    4   An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.
// QProcess::ReadError 3   An error occurred when attempting to read from the process. For example, the process may not be running.
// QProcess::UnknownError
    switch(error) {
    case QProcess::FailedToStart:
        qWarning() << "QProcess::FailedToStart";
        break;
    case QProcess::Crashed:
        qWarning() << "QProcess::Crashed";
        break;
    case QProcess::Timedout:
        qWarning() << "QProcess::Timedout";
        break;
    case QProcess::WriteError:
        qWarning() << "QProcess::WriteError";
        break;
    case QProcess::ReadError:
        qWarning() << "QProcess::ReadError";
        break;
    case QProcess::UnknownError:
        qWarning() << "QProcess::UnknownError";
        break;
    }
}

void ClickUpdateChecker::handleMetadataClickTokenObtained(const ClickUpdateMetadata *meta)
{
    qWarning() << "click checker: handling obtained token on metadata" << meta->name();
    // Pass the shared pointer instead.
    Q_EMIT updateAvailable(m_metas.value(meta->name()));

    // Check if all tokens are fetched.
    foreach (const QString &name, m_metas.keys()) {
        if (m_metas.value(name)->clickToken().isEmpty()) {
            return; // Not done.
        }
    }

    // Cache this data.
    cacheClickMetadata();

    // All metas had signed download urls, so we're done.
    Q_EMIT checkCompleted();
}

void ClickUpdateChecker::handleClickTokenRequestFailed(const ClickUpdateMetadata *meta)
{
    m_metas.remove(meta->name());
}

void ClickUpdateChecker::cacheClickMetadata()
{

}

void ClickUpdateChecker::requestClickMetadata()
{
    qWarning() << "click checker: asking for remote metadata...";
    QJsonObject serializer;

    // Construct the “name” list
    QJsonArray array;
    foreach (const QString &name, m_metas.keys())
        array.append(QJsonValue(name));
    serializer.insert("name", array);

    // Create list of frameworks.
    std::stringstream frameworks;
    for (auto f: Helpers::getAvailableFrameworks()) {
        frameworks << "," << f;
    }

    QJsonDocument doc(serializer);
    QByteArray content = doc.toJson();

    QString urlApps = Helpers::clickMetadataUrl();
    qWarning() << "click checker: using url" << urlApps;
    QString authHeader = m_token.signUrl(urlApps,
                                         QStringLiteral("POST"), true);
    QUrl url(urlApps);
    url.setQuery(authHeader);

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(
                QByteArray("X-Ubuntu-Frameworks"),
                QByteArray::fromStdString(frameworks.str()));
    request.setRawHeader(
                QByteArray("X-Ubuntu-Architecture"),
                QByteArray::fromStdString(Helpers::getArchitecture()));
    request.setUrl(url);

    QNetworkReply *reply = m_nam.post(request, content);
    initializeReply(reply);
}

void ClickUpdateChecker::requestSucceeded(QNetworkReply *reply)
{
    qWarning() << "click checker: request succeeded...";

    QJsonParseError *jsonError = new QJsonParseError;
    auto document = QJsonDocument::fromJson(reply->readAll(), jsonError);

    if (document.isArray()) {
        QJsonArray array = document.array();
        for (int i = 0; i < array.size(); i++) {
            auto object = array.at(i).toObject();
            auto name = object["name"].toString();
            qWarning() << "click checker: got metadata for" << name;
            auto version = object["version"].toString();
            auto icon_url = object["icon_url"].toString();
            auto url = object["download_url"].toString();
            auto download_sha512 = object["download_sha512"].toString();
            auto changelog = object["changelog"].toString();
            auto size = object["binary_filesize"].toInt();
            auto title = object["title"].toString();
            if (m_metas.contains(name)) {
                QSharedPointer<ClickUpdateMetadata> meta = m_metas.value(name);
                meta->setRemoteVersion(version);
                if (meta->isUpdateRequired()) {
                    meta->setIconUrl(icon_url);
                    meta->setDownloadUrl(url);
                    meta->setBinaryFilesize(size);
                    meta->setDownloadSha512(download_sha512);
                    meta->setChangelog(changelog);
                    meta->setTitle(title);

                    // ClickUpdateMetadata now has enough information to
                    // request the signed download URL, which we'll
                    // monitor and act on when it changes.
                    // See: handleDownloadUrlSigned
                    meta->requestClickToken();
                }
            }
        }
    } else {
        // Document wasn't an array, what was it?
        setErrorString("json parse failed: " + jsonError->errorString());
        Q_EMIT serverError();
    }

    delete jsonError;
    reply->deleteLater();
}

} // UpdatePlugin
