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
#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QSharedPointer>
#include <QDebug>

#include <token.h>
#include <ssoservice.h>
#include "clickupdatemetadata.h"
#include "clickupdatechecker.h"
#include "systemimage.h"

// Having the full namespaced name in a slot seems to confuse
// SignalSpy so we need this declaration.
using UbuntuOne::Token;

namespace UpdatePlugin {

//
// Mediator for the UI, System Image and Click updates.
//
// Note: UpdateManager does not provide models for click updates,
// nor system updates. The respective states are kept in
// ubuntu-download-manager and systemimage.
//
class UpdateManager : public QObject
{
    Q_OBJECT
public:
    Q_ENUMS(UpdateMode)
    Q_ENUMS(UpdateStatus)
    Q_ENUMS(ManagerStatus)

    enum UpdateMode {
        UpdateDownloadable,
        UpdateInstallable,
        UpdateInstallableWithRestart,
        UpdatePausable,
        UpdateNonPausable,
        UpdateRetriable
    };

    enum ManagerStatus {
        ManagerIdle,
        ManagerCheckingClickUpdates,
        ManagerCheckingSystemUpdates,
        ManagerCheckingAllUpdates,
        ManagerFailed
    };

    enum UpdateStatus {
        UpdateNotAvailable,
        UpdateNotStarted,
        UpdateAutomaticallyDownloading,
        UpdateManuallyDownloading,
        UpdateDownloadPaused,
        UpdateInstallationPaused,
        UpdateInstalling,
        UpdateInstalled,
        UpdateFailed
    };

    static UpdateManager *instance();

    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool authenticated READ authenticated
               NOTIFY authenticatedChanged)
    Q_PROPERTY(bool haveSufficientPower READ haveSufficientPower
               WRITE setHaveSufficientPower
               NOTIFY haveSufficientPowerChanged)
    Q_PROPERTY(QObject *udm READ udm WRITE setUdm NOTIFY udmChanged)
    Q_PROPERTY(int updatesCount READ updatesCount NOTIFY updatesCountChanged)
    Q_PROPERTY(int downloadMode READ downloadMode WRITE setDownloadMode
               NOTIFY downloadModeChanged)
    Q_PROPERTY(ManagerStatus managerStatus READ managerStatus NOTIFY managerStatusChanged)

    bool online() const;
    void setOnline(const bool online);

    bool authenticated() const;

    bool haveSufficientPower() const;
    void setHaveSufficientPower(const bool haveSufficientPower);

    QObject *udm() const;
    void setUdm(QObject *udm);
    void initializeUdm();

    int updatesCount() const;
    int downloadMode();
    void setDownloadMode(const int &downloadMode);

    ManagerStatus managerStatus() const;

protected:
    explicit UpdateManager(QObject *parent = 0);
    ~UpdateManager();

public slots:
    void checkForUpdates();
    void abortCheckForUpdates();

private slots:
    void downloadClickUpdate(
            const QSharedPointer<ClickUpdateMetadata> &meta);

    // System Image slots
    void handleSiAvailableStatus(const bool isAvailable,
                                 const bool downloading,
                                 const QString &availableVersion,
                                 const int &updateSize,
                                 const QString &lastUpdateDate,
                                 const QString &errorReason);
    // void siUpdateFailed(const int &consecutiveFailureCount,
    //                     const QString &lastReason);

    // SSO slots
    void handleCredentialsFound(const Token &token);
    void handleCredentialsFailed();

    void handleClickCheckCompleted();
    void updateClickUpdatesCount();

signals:
    void onlineChanged();
    void authenticatedChanged();
    void haveSufficientPowerChanged();
    void downloadModeChanged();
    void udmChanged();
    void updatesCountChanged();
    void managerStatusChanged();

    // void requestClickUpdateMetadata();
    // void requestInstalledClicks();


private:
    static UpdateManager *m_instance;

    void setAuthenticated(const bool authenticated);
    void setUpdatesCount();
    void setManagerStatus(const ManagerStatus &status);

    // Whether or not the click update metadata exist in UDM.
    bool clickUpdateInUdm(const QSharedPointer<ClickUpdateMetadata> &meta) const;

    // Creates the download in UDM for click update with given metadata.
    void createClickUpdateDownload(const QSharedPointer<ClickUpdateMetadata> &meta);

    void initializeSystemImage();
    void initializeClickUpdateChecker();
    void initializeSSOService();

    bool m_online;
    bool m_authenticated;
    bool m_haveSufficientPower;
    int m_updatesCount;
    int m_clickUpdatesCount;
    int m_systemUpdatesCount;
    ManagerStatus m_managerStatus;

    ClickUpdateChecker m_clickUpChecker;
    QSystemImage m_systemImage;
    QObject *m_udm;
    UbuntuOne::Token m_token;
    UbuntuOne::SSOService m_ssoService;
};

}

#endif // UPDATEMANAGER_H
