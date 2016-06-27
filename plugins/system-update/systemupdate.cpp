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

namespace UpdatePlugin
{
SystemUpdate *SystemUpdate::m_instance = 0;

SystemUpdate *SystemUpdate::instance()
{
    if (!m_instance) m_instance = new SystemUpdate;
    return m_instance;
}

SystemUpdate::SystemUpdate(QObject *parent)
    : QObject(parent)
    , m_updateDb(this)
{
}

// void SystemUpdate::notifyModelChanged()
// {
//     Q_EMIT (modelChanged());
// }

// void SystemUpdate::notifyModelItemChanged(const QString &id,
//                                           const int &revision)
// {
//     Q_EMIT (modelItemChanged(id, revision));
// }

// void SystemUpdate::notifyModelItemChanged(const QString &downloadId)
// {
//     Q_EMIT (modelItemChanged(downloadId));
// }

UpdateDb* SystemUpdate:: updateDb()
{
    return &m_updateDb;
}
} // UpdatePlugin
