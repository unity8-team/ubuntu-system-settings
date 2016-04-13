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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>

#include <assert.h>
#include "clickupdatechecker.h"
#include "helpers.h"


namespace UpdatePlugin {

ClickUpdateChecker::ClickUpdateChecker(QObject *parent):
    ClickApiProto(parent)
{
    initializeProcess();
}

ClickUpdateChecker::~ClickUpdateChecker()
{
    if (m_reply)
        delete m_reply;
}

void ClickUpdateChecker::initializeMeta(const QSharedPointer<ClickUpdateMetadata> &meta)
{
    QObject::connect(meta.data(), SIGNAL(credentialError()),
                     this, SIGNAL(credentialError()));
    QObject::connect(meta.data(), SIGNAL(clickTokenRequestSucceeded(const ClickUpdateMetadata&)),
                     this, SLOT(handleMetadataClickTokenObtained(const ClickUpdateMetadata&)));
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
// updated click (signing the download URL). Each of these can fail in various
// ways, so we report either serverError or networkError with an accompanying
// errorString.
//
// The first operation is obtaining the list of clicks installed on the system.
// The second is receiving the metadata for clicks we found installed.
// The third is signing the download url for each click update. We ask the
// ClickUpdateMetadata to do this for us.
void ClickUpdateChecker::checkForUpdates()
{
    // No token, so don't check anything, but report that the check
    // completed. We should not be given an invalid token here,
    // and we've yet to talk to the server so we can't know
    // the state of the token.
    if (!m_token.isValid()) {
        Q_EMIT checkCompleted();
        return;
    }

    // Clear list of click update metadatas; we're starting anew.
    m_metas.clear();

    setErrorString("");

    // Start process of getting the list of installed clicks.
    QStringList args("list");
    args << "--manifest";
    QString command = Helpers::whichClick();
    m_process.start(command, args);
}

// Tries to shut down all checks we might currently be doing.
void ClickUpdateChecker::abortCheckForUpdates()
{
    // Abort all click update metadata objects.
    foreach (const QString &name, m_metas.keys())
        m_metas.value(name)->abort();

    m_process.terminate();

    if (m_reply) {
        m_reply->abort();
        delete m_reply;
    }

    Q_EMIT checkCompleted();
}

void ClickUpdateChecker::handleInstalledClicks(const int &exitCode)
{
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
        Q_EMIT checkCompleted();
        return;
    }

    int i;
    for (i = 0; i < array.size(); i++) {
        QJsonObject object = array.at(i).toObject();

        QSharedPointer<ClickUpdateMetadata> meta(new ClickUpdateMetadata);

        meta->setToken(m_token);
        meta->setName(object.value("name").toString());
        meta->setTitle(object.value("title").toString());
        meta->setLocalVersion(object.value("version").toString());
        m_metas.insert(meta->name(), meta);
    }

    // Populate each ClickUpdateMetadata with remote metadata.
    requestClickMetadata();
}

void ClickUpdateChecker::handleMetadataClickTokenObtained(const ClickUpdateMetadata *meta)
{
    // Pass the shared pointer instead.
    foreach (const QString &name, m_metas.keys()) {
        if (name == meta->name()) {
            Q_EMIT clickUpdateDownloadable(m_metas.value(name));
            // not finished
            return;
        }
    }

    // Are we finished?
    foreach (const QString &name, m_metas.keys()) {
        if (m_metas.value(name)->clickToken().isEmpty()) {
            // not finished
            return;
        }
    }

    // All metas had signed download urls, so we're done.
    Q_EMIT checkCompleted();
}

void ClickUpdateChecker::handleClickTokenRequestFailed(const ClickUpdateMetadata *meta)
{
    m_metas.remove(meta->name());
}

void ClickUpdateChecker::requestClickMetadata()
{
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

    QString urlApps = Helpers::clickMetadataURL();
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

    m_reply = m_nam.post(request, content);

    // Wait for the reply to complete in some way, and set up listeners
    setUpReply();
}

void ClickUpdateChecker::requestSucceeded()
{
    // the scoped pointer will take care of calling the deleteLater when
    // leaving the slot
    auto r = qobject_cast<QNetworkReply*>(sender());

    // check for http error status and emit all the required signals
    if (!validReply(r)) {
        return;
    }

    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(r);

    QJsonParseError *jsonError = new QJsonParseError;
    auto document = QJsonDocument::fromJson(reply->readAll(), jsonError);

    if (document.isArray()) {
        QJsonArray array = document.array();
        for (int i = 0; i < array.size(); i++) {
            auto object = array.at(i).toObject();
            auto name = object["name"].toString();
            auto version = object["version"].toString();
            auto icon_url = object["icon_url"].toString();
            auto url = object["download_url"].toString();
            auto download_sha512 = object["download_sha512"].toString();
            auto changelog = object["changelog"].toString();
            auto size = object["binary_filesize"].toInt();
            if (m_metas.contains(name)) {
                QSharedPointer<ClickUpdateMetadata> meta = m_metas.value(name);
                meta->setRemoteVersion(version);
                if (meta->isUpdateRequired()) {
                    meta->setIconUrl(icon_url);
                    meta->setDownloadUrl(url);
                    meta->setBinaryFilesize(size);
                    meta->setDownloadSha512(download_sha512);
                    meta->setChangelog(changelog);

                    // ClickUpdateMetadata now has enough information to
                    // request the signed download URL, which we'll
                    // monitor and act on when it changes.
                    // See: handleDownloadUrlSigned
                    meta->requestClickToken();
                }
            }
        }
    }

    // Document wasn't an array, what was it?
    setErrorString("json parse failed: " + jsonError->errorString());
    delete jsonError;
    Q_EMIT serverError();
}

} // UpdatePlugin
