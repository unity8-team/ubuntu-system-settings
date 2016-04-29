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
#include "clickapiclient.h"

#include <assert.h>

namespace UpdatePlugin
{

ClickApiClient::ClickApiClient(QObject *parent) :
        QObject(parent),
        m_errorString(""),
        m_token(UbuntuOne::Token())
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
            this,
            SLOT(requestSslFailed(QNetworkReply *, const QList<QSslError>&)));
}

void ClickApiClient::initializeReply(QNetworkReply *reply)
{
    qWarning() << "click proto: init reply" << reply;
    connect(this, SIGNAL(abortNetworking()), reply, SLOT(abort()));
}
void ClickApiClient::setToken(const UbuntuOne::Token &token)
{
    m_token = token;
}

void ClickApiClient::requestSslFailed(QNetworkReply *reply,
                                      const QList<QSslError> &errors)
{
    QString errorString = "SSL error: ";
    foreach (const QSslError &err, errors){
    errorString += err.errorString();
}
    setErrorString(errorString);
    Q_EMIT serverError();
    reply->deleteLater();
}

void ClickApiClient::requestFinished(QNetworkReply *reply)
{
    qWarning() << "click proto: something finished" << reply;
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
    qWarning() << "click proto: HTTP Status: " << httpStatus;

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
