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
#include "clickapiproto.h"

namespace UpdatePlugin {

ClickApiProto::ClickApiProto(QObject *parent):
    QObject(parent)
{
}

ClickApiProto::~ClickApiProto()
{
}

void ClickApiProto::setUpReply()
{

    assert((m_reply != NULL) && "setUpReply got null reply!");

    connect(m_reply, SIGNAL(finished()),
            this, SLOT(requestSucceeded()));
    connect(m_reply, SIGNAL(sslErrors(const QList<QSslError> &errors)),
            this, SLOT(requestSslFailed(const QList<QSslError> &errors)));
    connect(m_reply, SIGNAL(error(const QNetworkReply::NetworkError &code)),
            SLOT(requestFailed(const QNetworkReply::NetworkError &code)));
}

void ClickApiProto::setToken(const UbuntuOne::Token &token)
{
    m_token = token;
}

void ClickApiProto::requestSslFailed(const QList<QSslError> &errors)
{
    auto reply = sender();
    QString errorString = "SSL error:";
    foreach (const QSslError &err, errors) {
        errorString += err.errorString();
    }
    setErrorString(errorString);
    Q_EMIT serverError();
    reply->deleteLater();
    m_reply = 0;
}

void ClickApiProto::onReplyError(const QNetworkReply::NetworkError &code)
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

bool ClickApiProto::validReply(const QNetworkReply *reply)
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

QString ClickApiProto::errorString() const
{
    return m_errorString;
}

void ClickApiProto::setErrorString(const QString &errorString)
{
    m_errorString = errorString;
    Q_EMIT errorStringChanged();
}

} // UpdatePlugin
