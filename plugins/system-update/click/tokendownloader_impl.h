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

#ifndef CLICK_TOKEN_DOWNLOADER_IMPL_H
#define CLICK_TOKEN_DOWNLOADER_IMPL_H

#include "click/client.h"
#include "click/tokendownloader.h"
#include "click/sessiontoken_impl.h"

namespace UpdatePlugin
{
namespace Click
{
class TokenDownloaderImpl : public TokenDownloader
{
    Q_OBJECT
public:
    explicit TokenDownloaderImpl(Client *client,
                                 QSharedPointer<Update> update,
                                 QObject *parent = nullptr);
    virtual ~TokenDownloaderImpl();
    virtual void download() override;
    virtual void setSessionToken(SessionToken &sessionToken) override;
    virtual Client* client() const override;
public Q_SLOTS:
    virtual void cancel() override;
protected Q_SLOTS:
    void handleSuccess(const QString &token);
    void handleFailure();
private:
    SessionToken m_sessionToken;
};
} // Click
} // UpdatePlugin

#endif // CLICK_TOKEN_DOWNLOADER_IMPL_H
