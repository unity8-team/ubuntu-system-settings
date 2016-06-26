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

#ifndef CLICK_UPDATE_H
#define CLICK_UPDATE_H

#include "update.h"
#include "clickapiclient.h"

#include <token.h>

#include <QObject>
#include <QNetworkReply>

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin
{
class ClickUpdate : public Update
{
    Q_OBJECT
public:
    explicit ClickUpdate(QObject *parent = 0);
    ~ClickUpdate();

    void requestClickToken();
    void cancel();
    void setU1Token(const UbuntuOne::Token &token);

private slots:
    void handleToken(QNetworkReply *reply);
    void handleTokenChanged();
//     void tokenRequestSslFailed(const QList<QSslError> &errors);
//     void tokenRequestFailed(const QNetworkReply::NetworkError &code);
//     void tokenRequestSucceeded(const QNetworkReply* reply);

signals:
    void clickTokenRequestSucceeded(const ClickUpdate *update);
    void clickTokenRequestFailed(ClickUpdate *update);
    // void credentialError();

private:
    ClickApiClient m_apiClient;
    UbuntuOne::Token m_u1Token;
};
} // UpdatePlugin

#endif // CLICK_UPDATE_H
