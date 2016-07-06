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

#include "systemupdate.h"
#include "networkaccessmanager.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin
{
class ClickApiClient : public QObject
{
    Q_OBJECT
public:
    explicit ClickApiClient(NetworkAccessManager *nam =
                                SystemUpdate::instance()->nam(),
                            QObject *parent = 0);
    ~ClickApiClient();

    void cancel();

    void getMetadata(const QUrl &url, const QByteArray &packageNames);
    void getToken(const QUrl &url);

private slots:
    void requestSucceeded(QNetworkReply *reply);
    void requestFinished(QNetworkReply *reply);
    void requestSslFailed(QNetworkReply *reply, const QList<QSslError> &errors);
    // void handleTokenReply(QNetworkReply *reply);
    // void handleMetadataReply(QNetworkReply *reply);

signals:
    void metadataRequestSucceeded(const QByteArray &metadata);
    void tokenRequestSucceeded(const QString &token);
    void networkError();
    void serverError();
    void credentialError();

    void abortNetworking();

private:
    void initializeReply(QNetworkReply *reply);
    bool validReply(const QNetworkReply *reply);
    NetworkAccessManager *m_nam;

    void initializeNam();
};
} // UpdatePlugin

#endif // CLICK_API_CLIENT_H
