/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "helpers.h"

#include "click/apiclient_impl.h"
#include "network/accessmanager_impl.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QScopedPointer>

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin
{
namespace Click
{
ApiClientImpl::ApiClientImpl(Network::Manager *nam, QObject *parent)
    : ApiClient(parent)
    , m_nam(nam)
{
    connect(m_nam, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(requestFinished(QNetworkReply *)));
    connect(m_nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError>&)),
            this, SLOT(requestSslFailed(QNetworkReply *, const QList<QSslError>&)));
}

ApiClientImpl::~ApiClientImpl()
{
    cancel();
}

void ApiClientImpl::requestMetadata(const QUrl &url,
                                    const QList<QString> &packages)
{
    // Create list of frameworks.
    std::stringstream frameworks;
    Q_FOREACH(auto f, Helpers::getAvailableFrameworks()) {
        frameworks << "," << f;
    }

    // Create JSON bytearray of packages.
    QJsonObject serializer;
    QJsonArray array;
    Q_FOREACH(auto name, packages) {
        array.append(QJsonValue(name));
    }
    serializer.insert("name", array);

    QJsonDocument doc(serializer);
    QByteArray content = doc.toJson();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QByteArray("X-Ubuntu-Frameworks"),
            QByteArray::fromStdString(frameworks.str()));
    request.setRawHeader(QByteArray("X-Ubuntu-Architecture"),
            QByteArray::fromStdString(Helpers::getArchitecture()));
    request.setUrl(url);
    request.setOriginatingObject(this);
    request.setAttribute(QNetworkRequest::User, "metadata-request");

    initializeReply(m_nam->post(request, content));
}

void ApiClientImpl::requestToken(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    request.setOriginatingObject(this);
    request.setAttribute(QNetworkRequest::User, "token-request");
    initializeReply(m_nam->head(request));
}

void ApiClientImpl::initializeReply(QNetworkReply *reply)
{
    connect(this, SIGNAL(abortNetworking()), reply, SLOT(abort()));
}

void ApiClientImpl::requestSslFailed(QNetworkReply *reply,
                                     const QList<QSslError> &errors)
{
    QString errorString = "SSL error: ";
    foreach (const QSslError &err, errors) {
        errorString += err.errorString();
    }
    qCritical() << Q_FUNC_INFO << errorString;
    Q_EMIT serverError();
    reply->deleteLater();
}

void ApiClientImpl::requestFinished(QNetworkReply *reply)
{
    if (reply->request().originatingObject() != this) {
        return; // We did not create this request.
    }

    if (!validReply(reply)) {
        // Error signals are already sent.
        reply->deleteLater();
        return;
    }

    switch (reply->error()) {
    case QNetworkReply::NoError:
        // Note that requestSucceeded will delete the reply.
        requestSucceeded(reply);
        return;
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
}

void ApiClientImpl::requestSucceeded(QNetworkReply *reply)
{
    QString rtp = reply->request().attribute(QNetworkRequest::User).toString();
    if (rtp == "token-request") {
        if (reply->hasRawHeader(X_CLICK_TOKEN)) {
            QString header(reply->rawHeader(X_CLICK_TOKEN));
            Q_EMIT tokenRequestSucceeded(header);
        } else {
            Q_EMIT tokenRequestSucceeded("");
        }
    } else if (rtp == "metadata-request") {
        handleMetadataReply(reply);
    } else {
        // We are not to handle this reply, so do an early return.
        return;
    }

    reply->deleteLater();
}

void ApiClientImpl::handleMetadataReply(QNetworkReply *reply)
{
    QScopedPointer<QJsonParseError> jsonError(new QJsonParseError);
    auto document = QJsonDocument::fromJson(reply->readAll(),
                                            jsonError.data());

    if (document.isArray()) {
        Q_EMIT metadataRequestSucceeded(document.array());
    } else {
        qCritical() << Q_FUNC_INFO << "Got invalid click metadata.";
        Q_EMIT serverError();
    }

    if (jsonError->error != QJsonParseError::NoError) {
        qCritical() << Q_FUNC_INFO << "Could not parse click metadata:"
                    << jsonError->errorString();
        Q_EMIT serverError();
    }
}

bool ApiClientImpl::validReply(const QNetworkReply *reply)
{
    auto statusAttr = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusAttr.isValid()) {
        Q_EMIT networkError();
        qCritical() << Q_FUNC_INFO << "Could not parse status code.";
        return false;
    }

    int httpStatus = statusAttr.toInt();

    if (httpStatus == 401 || httpStatus == 403) {
        qCritical() << Q_FUNC_INFO
                    << QString("Server responded with %1.").arg(httpStatus);
        Q_EMIT credentialError();
        return false;
    }

    if (httpStatus == 404) {
        qCritical() << Q_FUNC_INFO << "Server responded with 404.";
        Q_EMIT serverError();
        return false;
    }

    return true;
}

void ApiClientImpl::cancel()
{
    // Tell each reply to abort. See initializeReply().
    Q_EMIT abortNetworking();
}
} // Click
} // UpdatePlugin
