/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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
#include "systemupdate.h"
#include "network/manager_impl.h"

#include <QDateTime>

namespace UpdatePlugin
{
SystemUpdate *SystemUpdate::m_instance = 0;

SystemUpdate *SystemUpdate::instance()
{
    if (!m_instance) m_instance = new SystemUpdate;
    return m_instance;
}

void SystemUpdate::destroyInstance()
{
    delete m_instance;
    m_instance = nullptr;
}

SystemUpdate::SystemUpdate(QObject *parent)
    : QObject(parent)
    , m_db(new UpdateDb(this))
    , m_nam(new Network::ManagerImpl(this))
{
}

SystemUpdate::SystemUpdate(UpdateDb *db, Network::Manager *nam,
                           QObject *parent)
    : QObject(parent)
    , m_db(db)
    , m_nam(nam)
{
}

UpdateDb* SystemUpdate::db()
{
    return m_db;
}

Network::Manager* SystemUpdate::nam()
{
    return m_nam;
}

bool SystemUpdate::isCheckRequired()
{
    // Spec says that a manual check should not happen if a check was
    // completed less than 30 minutes ago.
    QDateTime now = QDateTime::currentDateTimeUtc().addSecs(-1800); // 30 mins
    return m_db->lastCheckDate() < now;
}

void SystemUpdate::checkCompleted()
{
    m_db->setLastCheckDate(QDateTime::currentDateTime());
}
} // UpdatePlugin
