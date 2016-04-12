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

namespace UpdatePlugin {

class UpdateManager : public QObject
{
    Q_OBJECT
public:
    static UpdateManager *instance();

    Q_ENUMS(UpdateMode)
    Q_ENUMS(UpdateStatus)
    Q_ENUMS(ManagerStatus)
    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool authenticated READ authenticated
               NOTIFY authenticatedChanged)
    Q_PROPERTY(bool haveSufficientPower READ haveSufficientPower
               WRITE setHaveSufficientPower
               NOTIFY haveSufficientPowerChanged)
    Q_PROPERTY(Ubuntu:DownloadManager::UbuntuDownloadManager udm READ udm
               NOTIFY udmChanged)
    Q_PROPERTY(int updatesCount READ updatesCount NOTIFY updatesCountChanged)
    Q_PROPERTY(int downloadMode READ downloadMode WRITE setDownloadMode NOTIFY downloadModeChanged)
    Q_PROPERTY(ManagerStatus managerStatus READ managerStatus NOTIFY managerStatusChanged)

    bool online() const;
    void setOnline(const bool online);

    bool authenticated() const;

    bool haveSufficientPower() const;
    void setHaveSufficientPower(const bool haveSufficientPower);

    Ubuntu:DownloadManager::UbuntuDownloadManager udm() const;

    int updatesCount() const;
    int downloadMode() const;
    void setDownloadMode(const int &downloadMode);

    ManagerStatus managerStatus() const;

    enum UpdateMode {
        Downloadable,
        Installable,
        InstallableWithRestart,
        Pausable,
        NonPausable,
        Retriable
    };

    enum ManagerStatus {
        Idle,
        CheckingClickUpdates,
        CheckingSystemUpdates,
        CheckingAllUpdates,
        Failed
    };

    enum UpdateStatus {
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

protected:
    explicit UpdateManager(QObject *parent = 0);
    ~UpdateManager();

public slots:
    void checkForUpdates();
    void abortCheckForUpdates();

private slots:
    void handleClickUpdateMetadata(
            const ClickUpdateMetadata &clickUpdateMetadata);

    // System Image slots
    void handleSiAvailableStatus(const bool isAvailable,
                                 const bool downloading,
                                 const QString &availableVersion,
                                 const int &updateSize,
                                 const QString &lastUpdateDate,
                                 const QString &errorReason);
    // void siUpdateFailed(const int &consecutiveFailureCount,
    //                     const QString &lastReason);

    // UDM slots
    // void udmDownloadFinished(const Ubuntu:DownloadManager::SingleDownload &download);
    // void udmErrorFound(const Ubuntu:DownloadManager::SingleDownload &download);

    // SSO slots
    void handleCredentialsFound(const Token &token);
    void handleCredentialsFailed();

    void handleClickCheckCompleted();


signals:
    void onlineChanged();
    void authenticatedChanged();
    void haveSufficientPowerChanged();
    void udmChanged();
    void updatesCountChanged();
    void managerStatusChanged();

    // void requestClickUpdateMetadata();
    // void requestInstalledClicks();


private:
    static UpdateManager *m_instance;

    bool m_online;
    bool m_authenticated;
    bool m_haveSufficientPower;
    int m_updatesCount;
    ManagerStatus m_managerStatus;

    ClickUpdateChecker m_clickUpChecker;
    SystemUpdate m_systemUpdate;
    QSystemImage m_systemImage;
    Ubuntu:DownloadManager::UbuntuDownloadManager m_udm;
    UbuntuOne::SSOService m_ssoService;

    void setAuthenticated(const bool authenticated);
    void setUpdatesCount(const int &count);
    void setManagerStatus(const ManagerStatus &status);

    // Whether or not the click update metadata exist in UDM.
    bool clickUpdateInUdm(const ClickUpdateMetadata &clickUpdateMetadata) const;

    // Creates the download in UDM for click update with given metadata.
    void createClickUpdateDownload(const ClickUpdateMetadata &clickUpdateMetadata);

    void setUpSystemImage();
    void setUpClickUpdateChecker();
    void setUpSSOService();
    void setUpUdm();
};

}

#endif // UPDATEMANAGER_H
