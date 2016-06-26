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

    void cancel();
    QNetworkAccessManager* nam();
    void initializeReply(QNetworkReply *reply);

    bool validReply(const QNetworkReply *reply);
    void setErrorString(const QString &errorString);

public slots:
    void requestSucceeded(QNetworkReply *reply);
    void requestFinished(QNetworkReply *reply);
    void requestSslFailed(QNetworkReply *reply, const QList<QSslError> &errors);

signals:
    void success(QNetworkReply *reply);
    void errorStringChanged();
    void networkError();
    void serverError();
    void credentialError();

    void abortNetworking();

private:
    QString m_errorString;
    QNetworkAccessManager m_nam;

    void initializeNam();
};
} // UpdatePlugin

#endif // CLICK_API_CLIENT_H
