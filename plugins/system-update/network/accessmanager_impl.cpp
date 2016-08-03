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

#include "network/accessmanager_impl.h"

namespace UpdatePlugin
{
namespace Network
{
ManagerImpl::ManagerImpl(QObject *parent)
    : Manager(parent)
    , m_impl()
{
    connect(&m_impl, SIGNAL(finished(QNetworkReply *)),
            this, SIGNAL(finished(QNetworkReply *)));
    connect(&m_impl,
            SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError>&)),
            this, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError>&)));
}

QNetworkReply* ManagerImpl::post(const QNetworkRequest &request,
                                 const QByteArray &data)
{
    return m_impl.post(request, data);
}

QNetworkReply* ManagerImpl::head(const QNetworkRequest &request)
{
    return m_impl.head(request);
}
} // Network
} // UpdatePlugin
