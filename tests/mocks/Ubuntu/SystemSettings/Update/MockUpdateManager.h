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

#ifndef MOCK_UPDATE_MANAGER_H
#define MOCK_UPDATE_MANAGER_H

#include "MockSystemImage.h"
#include "MockClickUpdateModel.h"

#include <QDebug>

class MockUpdateManager: public QObject
{
Q_OBJECT
    public:

    Q_ENUMS(UpdateKind)
    Q_ENUMS(UpdateState)
    Q_ENUMS(Status)

    enum UpdateKind
    {
        KindApp,
        KindSystem,
        KindUnknown
    };

    enum UpdateState
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

    enum Status
    {
        StatusIdle,
        StatusCheckingClickUpdates,
        StatusCheckingSystemUpdates,
        StatusCheckingAllUpdates,
        StatusBatchMode, // Installing all updates
        StatusNetworkError,
        StatusServerError
    };

    static MockUpdateManager *instance();

    Q_PROPERTY(bool authenticated READ authenticated
               NOTIFY authenticatedChanged)
    Q_PROPERTY(MockClickUpdateModel* installedClickUpdates READ installedClickUpdates
               CONSTANT)
    Q_PROPERTY(MockClickUpdateModel* pendingClickUpdates READ pendingClickUpdates
               CONSTANT)

    bool authenticated();
    MockClickUpdateModel *installedClickUpdates();
    MockClickUpdateModel *pendingClickUpdates();

    Q_INVOKABLE void checkForClickUpdates();
    Q_INVOKABLE void cancelCheckForClickUpdates();
    Q_INVOKABLE void retryClickPackage(const QString &packageName, const int &revision);
    Q_INVOKABLE void clickUpdateInstalled(const QString &packageName, const int &revision);

    Q_INVOKABLE void mockAuthenticate(const bool authenticated); // mock only
    Q_INVOKABLE void mockServerError(); // mock only
    Q_INVOKABLE void mockNetworkError(); // mock only
    Q_INVOKABLE void mockClickUpdateCheckComplete(); // mock only

protected:
    explicit MockUpdateManager(QObject *parent = 0);
    ~MockUpdateManager();

signals:
    void authenticatedChanged();

    void networkError();
    void serverError();
    void clickUpdateCheckCompleted();

private:
    static MockUpdateManager *m_instance;
    bool m_authenticated;
};

#endif // MOCK_UPDATE_MANAGER_H
