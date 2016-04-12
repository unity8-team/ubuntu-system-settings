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
#ifndef CLICKAPIPROXY_H
#define CLICKAPIPROXY_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//
// Implements some behaviour and state for interacting with a remove click
// api. Meant to normalize the behaviour of 401 and 403 responses, as well
// as reporting errors.
//
class ClickApiProto : public QObject
{
    Q_OBJECT
public:
    explicit ClickApiProto(QObject *parent = 0);
    ~ClickApiProto();

    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    QString errorString() const;
    void setToken(const UbuntuOne::Token &token);

protected slots:
    virtual void requestSucceeded() = 0;
    void requestFailed(const QNetworkReply::NetworkError &code);
    void requestSslFailed();

signals:
    void errorStringChanged();
    void networkError();
    void serverError();
    void credentialError();

protected:
    void setUpReply();
    void validReply();
    void setErrorString(const QString &errorString);

protected:
    QString m_errorString;
    UbuntuOne::Token m_token;
    QNetworkAccessManager m_nam;
    QNetworkReply* m_reply;

#endif // CLICKAPIPROXY_H
