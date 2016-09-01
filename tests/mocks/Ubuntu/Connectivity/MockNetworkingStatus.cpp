/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014-2016 Canonical Ltd.
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
 *
 * Authors:
 *     Antti Kaijanmäki <antti.kaijanmaki@canonical.com>
 */

#include "MockNetworkingStatus.h"
#include <QDebug>

MockNetworkingStatus::MockNetworkingStatus(QObject *parent)
        : QObject(parent)
        , m_status(Status::Offline)
{
    qRegisterMetaType<MockNetworkingStatus::Limitations>();
    qRegisterMetaType<QVector<MockNetworkingStatus::Limitations>>();
    qRegisterMetaType<MockNetworkingStatus::Status>();
}

MockNetworkingStatus::~MockNetworkingStatus()
{}

QVector<MockNetworkingStatus::Limitations>
MockNetworkingStatus::limitations() const
{
    return m_limitations;
}

MockNetworkingStatus::Status
MockNetworkingStatus::status() const
{
    return m_status;
}

void MockNetworkingStatus::setStatus(MockNetworkingStatus::Status status)
{
    m_status = status;
    Q_EMIT (statusChanged(status));
    switch (status) {
    case MockNetworkingStatus::Offline:
    case MockNetworkingStatus::Connecting:
        Q_EMIT (onlineChanged(false));
        break;
    case MockNetworkingStatus::Online:
        Q_EMIT (onlineChanged(true));
        break;
    }
}

bool
MockNetworkingStatus::online() const
{
    return status() == Status::Online;
}

bool
MockNetworkingStatus::limitedBandwith() const
{
    return limitations().contains(Limitations::Bandwith);
}

void MockNetworkingStatus::setLimitedBandwidth(bool limited)
{
    if (limited) {
        m_limitations << Limitations::Bandwith;
    } else if (limitedBandwith()) {
        m_limitations.remove(m_limitations.indexOf(Limitations::Bandwith));
    }
}
