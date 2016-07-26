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

#include "click/client.h"
#include "network/accessmanager_impl.h"

namespace UpdatePlugin
{
namespace Click
{
class ClientImpl : public Client
{
    Q_OBJECT
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
    void requestSucceeded(QNetworkReply *reply);
    void requestFinished(QNetworkReply *reply);
    void requestSslFailed(QNetworkReply *reply,
                          const QList<QSslError> &errors);

private:
    void initializeReply(QNetworkReply *reply);
    bool validReply(const QNetworkReply *reply);
    void initializeNam();
    void handleMetadataReply(QNetworkReply *reply);

    UpdatePlugin::Network::Manager *m_nam;
};
} // Click
} // UpdatePlugin

#endif // CLICK_CLIENT_IMPL_H
