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

#ifndef CLICK_CLIENT_IMPL_H
#define CLICK_CLIENT_IMPL_H

#include "client.h"
#include "manager_impl.h"

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin
{
namespace Click
{
class ClientImpl : public Client
{
public:
    explicit ClientImpl(QObject *parent = 0);
    explicit ClientImpl(UpdatePlugin::Network::Manager *nam,
                        QObject *parent = 0);
    virtual ~ClientImpl();

    virtual void cancel() override;
    virtual void requestMetadata(const QUrl &url,
                                 const QList<QString> &packages) override;
    virtual void requestToken(const QUrl &url) override;

protected slots:
    virtual void requestSucceeded(QNetworkReply *reply) override;
    virtual void requestFinished(QNetworkReply *reply) override;
    virtual void requestSslFailed(QNetworkReply *reply,
                                  const QList<QSslError> &errors) override;

private:
    void initializeReply(QNetworkReply *reply);
    bool validReply(const QNetworkReply *reply);
    void initializeNam();

    UpdatePlugin::Network::Manager *m_nam;
};
} // Click
} // UpdatePlugin

#endif // CLICK_CLIENT_IMPL_H
