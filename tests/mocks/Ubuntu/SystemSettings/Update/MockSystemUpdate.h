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
#ifndef MOCK_SYSTEM_UPDATE_H
#define MOCK_SYSTEM_UPDATE_H

#include <QObject>

class MockSystemUpdate : public QObject
{
    Q_OBJECT
    Q_ENUMS(UpdateKind)
    Q_ENUMS(UpdateState)
    Q_ENUMS(SystemStatus)
public:
    static MockSystemUpdate *instance();

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
        StateInstallFinished,
        StateInstalled,
        StateDownloaded,
        StateFailed
    };

    enum class SystemStatus
    {
        StatusIdle,
        StatusCheckingClickUpdates,
        StatusCheckingSystemUpdates,
        StatusCheckingAllUpdates,
        StatusBatchMode, // Installing all updates
        StatusNetworkError,
        StatusServerError
    };

protected:
    MockSystemUpdate(QObject *parent = 0)
    {
        Q_UNUSED(parent);
    }
    ~MockSystemUpdate() {}
private:
    static MockSystemUpdate *m_instance;
};

#endif // MOCK_SYSTEM_UPDATE_H
