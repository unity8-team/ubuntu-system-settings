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

#include <QDebug>

namespace UpdatePlugin
{
class SystemUpdate : public QObject
{
    Q_OBJECT
    Q_ENUMS(UpdateKind)
    Q_ENUMS(UpdateState)
    Q_ENUMS(SystemStatus)
public:
    static SystemUpdate *instance();

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

public slots:
    void notifyStoreChanged();
    void notifyStoreItemChanged(const QString &id, const int &revision);

signals:
    void storeChanged();
    void storeItemChanged(const QString &id, const int &revision);

protected:
    explicit SystemUpdate(QObject *parent = 0);
    ~SystemUpdate() {}

private:
    static SystemUpdate *m_instance;
};

} // UpdatePlugin

// Q_DECLARE_METATYPE(UpdatePlugin::SystemUpdate::UpdateState)
#endif // SYSTEM_UPDATE_H
