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
#ifndef SYSTEM_UPDATE_H
#define SYSTEM_UPDATE_H

#include "updatedb.h"
#include "network/manager.h"

#include <QDebug>

namespace UpdatePlugin
{
class SystemUpdate : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
public:
    static SystemUpdate *instance();
    static void destroyInstance();

    enum class Status
    {
        StatusIdle,
        StatusCheckingClickUpdates,
        StatusCheckingSystemUpdates,
        StatusCheckingAllUpdates,
        StatusBatchMode, // Installing all updates
        StatusBatchModePaused, // Installing all updates paused
        StatusNetworkError,
        StatusServerError
    };

    UpdateDb* db();
    Network::Manager* nam();

protected:
    explicit SystemUpdate(QObject *parent = 0);
    ~SystemUpdate() { qWarning() << "SystemUpdate was destroyed."; }

private:
    static SystemUpdate *m_instance;
    UpdateDb *m_db;
    Network::Manager *m_nam;
};

} // UpdatePlugin
#endif // SYSTEM_UPDATE_H
