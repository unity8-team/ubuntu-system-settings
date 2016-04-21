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
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>

#include "clickupdatechecker.h"
#include "helpers.h"


namespace UpdatePlugin {

ClickUpdateChecker::ClickUpdateChecker(QObject *parent):
    ClickApiProto(parent),
    m_process(),
    m_metas(),
    m_apiCache(this)
{
    initializeProcess();
}

ClickUpdateChecker::~ClickUpdateChecker()
{
}

void ClickUpdateChecker::initializeMeta(const QSharedPointer<ClickUpdateMetadata> &meta)
{
    QObject::connect(meta.data(), SIGNAL(credentialError()),
                     this, SIGNAL(credentialError()));
    QObject::connect(meta.data(), SIGNAL(clickTokenRequestSucceeded(const ClickUpdateMetadata*)),
                     this, SLOT(processClickToken(const ClickUpdateMetadata*)));
    QObject::connect(meta.data(), SIGNAL(clickTokenRequestFailed()),
                     this, SLOT(handleClickTokenFailure()));
}

void ClickUpdateChecker::initializeProcess()
{
    QObject::connect(&m_process, SIGNAL(finished(const int&)),
                     this, SLOT(processInstalledClicks(const int&)));
}

int ClickUpdateChecker::cachedCount() {
    if (m_apiCache.valid()) {
        return m_apiCache.read().count();
    }
    return 0;
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

    m_metas.clear();
    setErrorString("");

    // Do we have cached data? If so, return it and mark the check as complete.
    if (m_apiCache.valid()) {
        qWarning() << "click checker: cache valid";
        foreach(const QSharedPointer<ClickUpdateMetadata> &meta, m_apiCache.read()){
            qWarning() << "click checker: cache had" << meta->name();
            m_metas.insert(meta->name(), meta);

            // For retries: set a token and store it for later.
            meta->setToken(m_token);
            initializeMeta(meta);

            Q_EMIT updateAvailable(meta);
        }
        Q_EMIT checkCompleted();
        return;
    }

    qWarning() << "click checker: cache invalid";

    // Start process of getting the list of installed clicks.
    QStringList args("list");
    args << "--manifest";
    QString command = Helpers::whichClick();
    m_process.start(command, args);
    if (!m_process.waitForStarted()) {
        handleProcessError(m_process.error());
    }
}

void ClickUpdateChecker::check(const QString &packageName)
{
    qWarning() << "click checker: checking this one file" << packageName << "...";
    if (m_metas.contains(packageName)) {
        qWarning() << "click checker: requesting click token for" << packageName << "...";

        // For now, assume this check means INSTALL!
        m_metas.value(packageName)->setAutomatic(true);
        m_metas.value(packageName)->requestClickToken();
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

void ClickUpdateChecker::processInstalledClicks(const int &exitCode)
{
    qWarning() << "click checker: processInstalledClicks..." << exitCode;
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
        QSharedPointer<ClickUpdateMetadata> meta(new ClickUpdateMetadata);
        meta->setToken(m_token);
        initializeMeta(meta);

        QJsonObject object = array.at(i).toObject();
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

void ClickUpdateChecker::processClickToken(const ClickUpdateMetadata *meta)
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

    m_apiCache.write(m_metas.values());

    // All metas had signed download urls, so we're done.
    Q_EMIT checkCompleted();
}

void ClickUpdateChecker::handleClickTokenFailure(const ClickUpdateMetadata *meta)
{
    m_metas.remove(meta->name());
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
        parseClickMetadata(document.array());
    } else {
        setErrorString("Got unexpected JSON from server.");
        Q_EMIT serverError();
    }

    if (jsonError->error != QJsonParseError::NoError) {
        setErrorString("json parse failed: " + jsonError->errorString());
        Q_EMIT serverError();
    }

    delete jsonError;
    reply->deleteLater();
}

void ClickUpdateChecker::parseClickMetadata(const QJsonArray &array)
{
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

                // Start the process of obtaining a click token for this
                // click update.
                meta->requestClickToken();
            }
        }
    }
}

} // UpdatePlugin
