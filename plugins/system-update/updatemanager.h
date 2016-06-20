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

#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#include <QDebug>

#include "updatestore.h"

namespace UpdatePlugin
{
class UpdateManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(UpdateKind)
    Q_ENUMS(UpdateState)
    Q_ENUMS(Status)
public:
    enum class UpdateKind
    {
        KindApp,
        KindSystem,
        KindUnknown
    };

    enum class UpdateState
    {
        StateUnknown,
        StateAvailable,
        StateUnavailable,
        StateQueuedForDownload,
        StateDownloading,
        StateDownloadingAutomatically,
        StateDownloadPaused,
        StateAutomaticDownloadPaused,
        StateInstalling,
        StateInstallingAutomatically,
        StateInstallPaused,
        StateInstalled,
        StateDownloaded,
        StateFailed
    };

    enum class Status
    {
        StatusIdle,
        StatusCheckingClickUpdates,
        StatusCheckingSystemUpdates,
        StatusCheckingAllUpdates,
        StatusBatchMode, // Installing all updates
        StatusNetworkError,
        StatusServerError
    };

    static UpdateManager *instance();
    UpdateStore *updateStore();

protected:
    explicit UpdateManager(QObject *parent = 0);
    ~UpdateManager();

private:
    static UpdateManager *m_instance;
    void initializeUpdateStore();
    UpdateStore m_updatestore;
};
} // UpdatePlugin

#endif // UPDATE_MANAGER_H
