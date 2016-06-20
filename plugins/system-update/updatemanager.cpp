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

#include "updatemanager.h"
#include "helpers.h"

namespace UpdatePlugin
{

UpdateManager *UpdateManager::m_instance = 0;

UpdateManager *UpdateManager::instance()
{
    if (!m_instance) m_instance = new UpdateManager;
    return m_instance;
}

UpdateManager::UpdateManager(QObject *parent) : QObject(parent)
{
    initializeUpdateStore();
}

UpdateManager::~UpdateManager()
{
}

void UpdateManager::initializeUpdateStore()
{
    qWarning() << "last click check" << m_updatestore.lastCheckDate().toString("dd.MM.yyyy hh:mm:ss");


    // qWarning()
    //         << "in initializeUpdateStore we set m_clickUpdatesCount to "
    //         << m_clickUpdatesCount;
    // calculateUpdatesCount();
}

UpdateStore *UpdateManager::updateStore()
{
    return &m_updatestore;
}

} // UpdatePlugin
