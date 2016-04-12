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
#include "clickupdatechecker.h"
#include "helper.h"


namespace UpdatePlugin {

ClickUpdateChecker::ClickUpdateChecker(QObject *parent)
{
    setUpProcess();
}

ClickUpdateChecker::~ClickUpdateChecker()
{
    if (m_reply)
        delete m_reply;
}

void ClickUpdateChecker::setUpMeta(const ClickUpdateMetadata &meta)
{
    connect(&meta, SIGNAL(credentialError()),
            this, SIGNAL(credentialError()));
    connect(&meta, SIGNAL(signedDownloadUrlChanged()),
            this, SLOT(handlePrimedMeta()));
}

void ClickUpdateChecker::setUpProcess()
{
    QObject::connect(&m_process,
                     SIGNAL(finished(const int &exitCode)),
                     this,
                     SLOT(processedInstalledClicks(const int &exitCode)));
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
    if (m_token == null) {
        Q_EMIT checkCompleted();
    }

    assert(m_token.isValid() && "ClickUpdateChecker got bad token!");

    // Clear list of click update metadatas; we're starting anew.
    m_metas.clear();

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
        m_metas.value(name).abort();

    m_process.terminate();

    if (m_reply) {
        m_reply.abort();
        delete m_reply;
    }
}

void ClickUpdateChecker::processedInstalledClicks(const int &exitCode)
{
    if (exitCode > 0) {
        setErrorString(
            QString(
                "list command exited with code %1."
            ).arg(exitCode));
        );
        Q_EMIT checkCompleted();
        return;
    }

    QString output(m_process.readAllStandardOutput());
    QJsonDocument document = QJsonDocument::fromJson(output.toUtf8());
    QJsonArray array = document.array();

    int i;
    for (i = 0; i < array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        ClickUpdateMetadata meta(this, m_token);

        meta.setName(object.value("name").toString());
        meta.setTitle(object.value("title").toString());
        meta.setLocalVersion(object.value("version").toString());
    }
}

void ClickUpdateChecker::clickUpdateMetadataSignedDownloadUrl()
{
    ClickUpdateMetadata *meta = qobject_cast<ClickUpdateMetadata*>(sender());
    foundClickUpdate(*meta);
    delete meta;
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
            QByteArray::fromStdString(getArchitecture()));
    request.setUrl(url);

    m_reply = m_nam.post(request, content);
    setUpReply();
}

void ClickUpdateChecker::setUpReply()
{

    assert((m_reply != NULL) && "setUpReply got null reply!");

    connect(m_reply, SIGNAL(finished()),
            this, SLOT(requestSucceeded()));
    connect(m_reply, SIGNAL(sslErrors(const QList<QSslError> &errors)),
            this, SLOT(requestSslFailed(const QList<QSslError> &errors)));
    connect(m_reply, SIGNAL(error(const QNetworkReply::NetworkError &code)),
            SLOT(requestFailed(const QNetworkReply::NetworkError &code)));
}

void ClickUpdateChecker::setToken(const UbuntuOne::Token &token)
{
    m_token = token;
}

void ClickUpdateChecker::requestSslFailed(const QList<QSslError> &errors)
{
    auto reply = sender();
    QString errorString = "SSL error:";
    foreach (const QSslError &err, errors) {
        errorString << err.errorString();
    }
    setErrorString(errorString);
    Q_EMIT serverError();
    reply->deleteLater();
    m_reply = 0;
}

void ClickUpdateChecker::onReplyError(const QNetworkReply::NetworkError &code)
{
    auto reply = sender();
    setErrorString("network error");
    switch (code) {
        case QNetworkReply::TemporaryNetworkFailureError:
        case QNetworkReply::UnknownNetworkError:
        case QNetworkReply::UnknownProxyError:
        case QNetworkReply::UnknownServerError:
            Q_EMIT networkError();
            break;
        default:
            Q_EMIT serverError();
    }
    reply->deleteLater();
    m_reply = 0;
}

void ClickUpdateChecker::requestSucceeded()
{
    // the scoped pointer will take care of calling the deleteLater when
    // leaving the slot
    auto r = qobject_cast<QNetworkReply*>(sender());

    // check for http error status and emit all the required signals
    if (!replyIsValid(r)) {
        return;
    }

    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(r);

    QJsonParseError jsonError;
    auto document = QJsonDocument::fromJson(reply->readAll(), jsonError);

    if (document.isArray()) {
        QJsonArray array = document.array();
        bool updates = false;
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
                ClickUpdateMetadata meta = m_metas.get(name);
                meta.setRemoteVersion(version);
                if (meta.isUpdateRequired()) {
                    meta.setIconUrl(icon_url);
                    meta.setDownloadUrl(url);
                    meta.setBinaryFilesize(size);
                    meta.setDownloadSha512(download_sha512);
                    meta.setChangelog(changelog);

                    // ClickUpdateMetadata now has enough information to
                    // request the signed download URL, which we'll
                    // monitor and act on when it changes.
                    // See: clickUpdateMetadataSignedDownloadUrl
                    meta.signDownloadUrl();
                }
            }
        }
    }

    // Document wasn't an array, what was it?
    setErrorString("json parse failed: " + jsonError.errorString());
    Q_EMIT serverError();
}

bool ClickUpdateChecker::validReply(QNetworkReply *reply)
{
    auto statusAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusAttr.isValid()) {
        Q_EMIT networkError();
        setErrorString("status attribute was invalid");
        return false;
    }

    int httpStatus = statusAttr.toInt();
    qWarning() << "HTTP Status: " << httpStatus;

    if (httpStatus == 401 || httpStatus == 403) {
        setErrorString("credential error");
        Q_EMIT credentialError();
        return false;
    }

    return true;
}

QString ClickUpdateChecker::errorString() const
{
    return m_errorString;
}

void ClickUpdateChecker::setErrorString(const QString &errorString)
{
    m_errorString = errorString;
    Q_EMIT errorStringChanged();
}

} // Namespace UpdatePlugin
