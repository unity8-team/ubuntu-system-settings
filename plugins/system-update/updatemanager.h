/*
 * Copyright (C) 2013-2016 Canonical Ltd
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
#ifndef PLUGINS_SYSTEM_UPDATE_MANAGER_H_
#define PLUGINS_SYSTEM_UPDATE_MANAGER_H_

#include <QDebug>
#include <QSharedPointer>
#include <QSqlQueryModel>

#include <token.h>
#include <ssoservice.h>

#include "clickupdatemetadata.h"
#include "clickupdatechecker.h"
#include "clickupdatestore.h"
#include "systemimage.h"

// Having the full namespaced name in a slot seems to confuse
// SignalSpy so we need this declaration.
using UbuntuOne::Token;

namespace UpdatePlugin
{

class UpdateManager: public QObject
{
Q_OBJECT
    public:

    Q_ENUMS(UpdateMode)
    Q_ENUMS(UpdateStatus)
    Q_ENUMS(ManagerStatus)
    enum UpdateMode
    {
        Downloadable,
        Installable,
        InstallableWithRestart,
        Pausable,
        Resumable,
        NonPausable,
        Retriable
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

    enum UpdateStatus
    {
        NotAvailable,
        NotStarted,
        AutomaticallyDownloading,
        ManuallyDownloading,
        DownloadPaused,
        InstallationPaused,
        Installing,
        Installed,
        Failed
    };

    static UpdateManager *instance();

    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool authenticated READ authenticated
            NOTIFY authenticatedChanged)
    Q_PROPERTY(bool haveSufficientPower READ haveSufficientPower
            WRITE setHaveSufficientPower
            NOTIFY haveSufficientPowerChanged)
    Q_PROPERTY(bool haveSystemUpdate READ haveSystemUpdate
            NOTIFY haveSystemUpdateChanged)
    Q_PROPERTY(QSystemImage* systemImageBackend READ systemImageBackend CONSTANT)
    Q_PROPERTY(int updatesCount READ updatesCount NOTIFY updatesCountChanged)
    Q_PROPERTY(int downloadMode READ downloadMode WRITE setDownloadMode
            NOTIFY downloadModeChanged)
    Q_PROPERTY(ManagerStatus managerStatus READ managerStatus
            NOTIFY managerStatusChanged)
    Q_PROPERTY(QSqlQueryModel* installedClickUpdates READ installedClickUpdates
               CONSTANT)
    Q_PROPERTY(QSqlQueryModel* activeClickUpdates READ activeClickUpdates
               CONSTANT)

    bool online() const;
    void setOnline(const bool online);
    bool authenticated() const;
    bool haveSystemUpdate() const;
    QSystemImage* systemImageBackend() const;
    bool haveSufficientPower() const;
    void setHaveSufficientPower(const bool haveSufficientPower);
    int updatesCount() const;
    int downloadMode();
    void setDownloadMode(const int &downloadMode);
    ManagerStatus managerStatus() const;

    QSqlQueryModel *installedClickUpdates();
    QSqlQueryModel *activeClickUpdates();

    Q_INVOKABLE void checkForUpdates();
    Q_INVOKABLE void cancelCheckForUpdates();
    Q_INVOKABLE void retryClickPackage(const QString &packageName);

protected:
    explicit UpdateManager(QObject *parent = 0);
    ~UpdateManager();

private slots:
    void onClickUpdateAvailable(
            const QSharedPointer<ClickUpdateMetadata> &meta);

    void handleSiAvailableStatus(const bool isAvailable, const bool downloading,
                                 const QString &availableVersion,
                                 const int &updateSize,
                                 const QString &lastUpdateDate,
                                 const QString &errorReason);
    // void siUpdateFailed(const int &consecutiveFailureCount,
    //                     const QString &lastReason);

    void handleCredentialsFound(const Token &token);
    void handleCredentialsFailed();

    void onClickCheckCompleted();
    void onNetworkError();
    void onServerError();

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
    static UpdateManager *m_instance;

    void setAuthenticated(const bool authenticated);
    void setManagerStatus(const ManagerStatus &status);

    // Creates the download in UDM for click update with given metadata.
    void createClickUpdateDownload(
            const QSharedPointer<ClickUpdateMetadata> &meta);
    void calculateUpdatesCount();

    void initializeSystemImage();
    void initializeClickUpdateChecker();
    void initializeSSOService();
    void initializeClickUpdateStore();

    bool m_online;
    bool m_authenticated;
    bool m_haveSufficientPower;
    int m_updatesCount;
    int m_clickUpdatesCount;
    int m_systemUpdatesCount;

    ManagerStatus m_managerStatus;
    ClickUpdateChecker m_clickUpChecker;
    QSystemImage *m_systemImage;
    UbuntuOne::Token m_token;
    UbuntuOne::SSOService m_ssoService;
    ClickUpdateStore m_clickUpdateStore;
};

}

#endif // PLUGINS_SYSTEM_UPDATE_MANAGER_H_
