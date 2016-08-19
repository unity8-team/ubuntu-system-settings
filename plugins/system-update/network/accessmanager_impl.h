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

#ifndef NETWORK_ACCESS_MANAGER_IMPL_H
#define NETWORK_ACCESS_MANAGER_IMPL_H

#include "network/accessmanager.h"

namespace UpdatePlugin
{
namespace Network
{
class ManagerImpl : public Manager
{
public:
    ManagerImpl(QObject *parent = nullptr);
    virtual ~ManagerImpl() {};
    virtual QNetworkReply* post(const QNetworkRequest &request, const QByteArray &data) override;
    virtual QNetworkReply* head(const QNetworkRequest &request) override;
private:
    QNetworkAccessManager m_impl;
};
} // Network
} // UpdatePlugin

#endif // NETWORK_ACCESS_MANAGER_IMPL_H
