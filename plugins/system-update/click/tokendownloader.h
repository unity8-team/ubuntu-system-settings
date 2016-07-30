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

#ifndef CLICK_TOKEN_DOWNLOADER_H
#define CLICK_TOKEN_DOWNLOADER_H

#include "update.h"
#include "click/client.h"

#include <token.h>

#include <QObject>
#include <QSharedPointer>

namespace UpdatePlugin
{
namespace Click
{
class TokenDownloader : public QObject
{
    Q_OBJECT
public:
    explicit TokenDownloader(Client *client,
                             QSharedPointer<Update> update,
                             QObject *parent = nullptr)
        : QObject(parent)
        , m_client(client)
        , m_update(update)
        , m_authToken(UbuntuOne::Token()) {}
    virtual ~TokenDownloader() {};

    // Downloads a token.
    virtual void download() = 0;

    // Set the authentication token..
    virtual void setAuthToken(const UbuntuOne::Token &authToken) = 0;
    virtual Client* client() const = 0;
public Q_SLOTS:
    // Cancel any ongoing token download.
    virtual void cancel() = 0;
Q_SIGNALS:
    // This signal is emitted when a token download succeeds.
    void downloadSucceeded(QSharedPointer<Update> update);

    // This signal is emitted when a token download fails.
    void downloadFailed(QSharedPointer<Update> update);
protected:
    Client *m_client;
    QSharedPointer<Update> m_update;
    UbuntuOne::Token m_authToken;
};
} // Click
} // UpdatePlugin

#endif // CLICK_TOKEN_DOWNLOADER_H
