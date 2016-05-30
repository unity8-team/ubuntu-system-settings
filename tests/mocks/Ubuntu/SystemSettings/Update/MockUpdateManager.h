/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Mediator for downloading and installing system and click updates.
 *
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
    Q_ENUMS(ManagerStatus)

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
        StateInstalling,
        StateInstallingAutomatically,
        StateInstallPaused,
        StateInstalled,
        StateDownloaded,
        StateFailed
    };

    enum ManagerStatus
    {
        Idle,
        CheckingClickUpdates,
        CheckingSystemUpdates,
        CheckingAllUpdates,
        BatchMode, // Installing all updates
        NetworkError,
        ServerError
    };

    static MockUpdateManager *instance();

    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool authenticated READ authenticated
            NOTIFY authenticatedChanged)
    Q_PROPERTY(bool haveSufficientPower READ haveSufficientPower
            WRITE setHaveSufficientPower
            NOTIFY haveSufficientPowerChanged)
    Q_PROPERTY(bool haveSystemUpdate READ haveSystemUpdate
            NOTIFY haveSystemUpdateChanged)
    Q_PROPERTY(MockSystemImage* systemImageBackend READ systemImageBackend CONSTANT)
    Q_PROPERTY(int updatesCount READ updatesCount NOTIFY updatesCountChanged)
    Q_PROPERTY(int downloadMode READ downloadMode WRITE setDownloadMode
            NOTIFY downloadModeChanged)
    Q_PROPERTY(ManagerStatus managerStatus READ managerStatus
            NOTIFY managerStatusChanged)
    Q_PROPERTY(MockClickUpdateModel* installedClickUpdates READ installedClickUpdates
               CONSTANT)
    Q_PROPERTY(MockClickUpdateModel* activeClickUpdates READ activeClickUpdates
               CONSTANT)

    bool online() const;
    void setOnline(const bool online);
    bool authenticated() const;
    bool haveSystemUpdate() const;
    MockSystemImage* systemImageBackend() const;
    bool haveSufficientPower() const;
    void setHaveSufficientPower(const bool haveSufficientPower);
    int updatesCount() const;
    int downloadMode();
    void setDownloadMode(const int &downloadMode);
    ManagerStatus managerStatus() const;

    MockClickUpdateModel *installedClickUpdates();
    MockClickUpdateModel *activeClickUpdates();

    Q_INVOKABLE void checkForUpdates();
    Q_INVOKABLE void cancelCheckForUpdates();
    Q_INVOKABLE void retryClickPackage(const QString &packageName, const int &revision);

protected:
    explicit MockUpdateManager(QObject *parent = 0);
    ~MockUpdateManager();

signals:
    void onlineChanged();
    void connected();
    void disconnected();

    void authenticatedChanged();
    void haveSufficientPowerChanged();
    void haveSystemUpdateChanged();
    void downloadModeChanged();
    void updatesCountChanged();
    void managerStatusChanged();
    void clickUpdateReady(const QString &url, const QString &hash,
                          const QString &algorithm,
                          const QVariantMap &metadata,
                          const QVariantMap &headers,
                          const bool autoStart);

private:
    static MockUpdateManager *m_instance;

    MockSystemImage *m_systemImage;
};

#endif // MOCK_UPDATE_MANAGER_H
