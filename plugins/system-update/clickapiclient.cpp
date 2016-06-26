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

#include "clickapiclient.h"

#include <assert.h>

namespace UpdatePlugin
{
ClickApiClient::ClickApiClient(QObject *parent)
    : QObject(parent)
    , m_errorString("")
{
    initializeNam();
}

ClickApiClient::~ClickApiClient()
{
    cancel();
}

void ClickApiClient::initializeNam()
{
    connect(&m_nam, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(requestFinished(QNetworkReply *)));
    connect(&m_nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError>&)),
            this, SLOT(requestSslFailed(QNetworkReply *, const QList<QSslError>&)));
}

QNetworkAccessManager* ClickApiClient::nam()
{
    return &m_nam;
}

void ClickApiClient::initializeReply(QNetworkReply *reply)
{
    qWarning() << "click api client: init reply" << reply;
    connect(this, SIGNAL(abortNetworking()), reply, SLOT(abort()));
}

void ClickApiClient::requestSslFailed(QNetworkReply *reply,
                                      const QList<QSslError> &errors)
{
    QString errorString = "SSL error: ";
    foreach (const QSslError &err, errors) {
        errorString += err.errorString();
    }
    setErrorString(errorString);
    Q_EMIT serverError();
    reply->deleteLater();
}

void ClickApiClient::requestFinished(QNetworkReply *reply)
{
    qWarning() << "click api client: something finished" << reply;
    // check for http error status and emit all the required signals
    if (!validReply(reply)) {
        reply->deleteLater();
        return;
    }
    qWarning() << "valid reply!";

    switch (reply->error()) {
    case QNetworkReply::NoError:
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

void ClickApiClient::requestSucceeded(QNetworkReply *reply)
{
    Q_EMIT (success(reply));
}

bool ClickApiClient::validReply(const QNetworkReply *reply)
{
    auto statusAttr = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusAttr.isValid()) {
        Q_EMIT networkError();
        qWarning() << "status attribute was invalid";
        setErrorString("Could not parse the HTTP status code.");
        return false;
    }

    int httpStatus = statusAttr.toInt();
    qWarning() << "click api client: HTTP Status: " << httpStatus;

    if (httpStatus == 401 || httpStatus == 403) {
        setErrorString(QString("Server responded with %1.").arg(httpStatus));
        Q_EMIT credentialError();
        return false;
    }

    if (httpStatus == 404) {
        setErrorString("Server responded with 404.");
        Q_EMIT serverError();
        return false;
    }

    return true;
}

QString ClickApiClient::errorString() const
{
    return m_errorString;
}

void ClickApiClient::setErrorString(const QString &errorString)
{
    m_errorString = errorString;
    Q_EMIT errorStringChanged();
}

void ClickApiClient::cancel()
{
    // Tell each reply to abort. See initializeReply().
    Q_EMIT abortNetworking();
}

} // UpdatePlugin
