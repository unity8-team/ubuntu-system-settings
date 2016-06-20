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

#ifndef CLICK_API_CLIENT_H
#define CLICK_API_CLIENT_H

#include <token.h>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace UpdatePlugin
{
class ClickApiClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
public:
    explicit ClickApiClient(QObject *parent = 0);
    ~ClickApiClient();

    QString errorString() const;

    // Set the token we will provide to the remote API for identification.
    void setToken(const UbuntuOne::Token &token);

    // Cancel each pending network reply.
    void cancel();

protected slots:
    virtual void requestSucceeded(QNetworkReply *reply) = 0;
    void requestFinished(QNetworkReply *reply);
    void requestSslFailed(QNetworkReply *reply, const QList<QSslError> &errors);

signals:
    void errorStringChanged();
    void networkError();
    void serverError();
    void credentialError();

    // This signal is emitted specifically for cancelling active network
    // replies.
    void abortNetworking();

protected:
    // Set up connections on a network reply.
    void initializeReply(QNetworkReply *reply);

    // Return whether or not the reply was valid. Emits signals as to indicate
    // what went wrong, if anything.
    bool validReply(const QNetworkReply *reply);
    void setErrorString(const QString &errorString);

    QString m_errorString;
    UbuntuOne::Token m_token;
    QNetworkAccessManager m_nam;

private:
    // Set up connections to the network manager.
    void initializeNam();
};
} // UpdatePlugin

#endif // CLICK_API_CLIENT_H
