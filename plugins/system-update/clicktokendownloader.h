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

#ifndef CLICKTOKEN_DOWNLOADER_H
#define CLICKTOKEN_DOWNLOADER_H

#include "clickapiclient.h"
#include "update.h"

#include <token.h>

#include <QObject>
#include <QSharedPointer>

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin
{
class ClickTokenDownloader : public QObject
{
    Q_OBJECT
public:
    explicit ClickTokenDownloader(QObject *parent, Update *update);
    ~ClickTokenDownloader();

    void requestToken();
    void setAuthToken(const UbuntuOne::Token &authToken);

signals:
    void tokenRequestSucceeded(Update *update);
    void tokenRequestFailed(Update *update);

private slots:
    void cancel();
    void handleSuccess(QNetworkReply *reply);

private:
    void init();
    Update *m_update;
    ClickApiClient m_apiClient;
    UbuntuOne::Token m_authToken;
};
} // UpdatePlugin

#endif // CLICKTOKEN_DOWNLOADER_H
