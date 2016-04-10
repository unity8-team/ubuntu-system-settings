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
    explicit UpdateManager(QObject *parent = 0);
    ~UpdateManager() {}
    static UpdateManager *instance();

    Q_ENUMS(UpdateStatus)
    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool authenticated READ authenticated
               NOTIFY authenticatedChanged)
    Q_PROPERTY(Ubuntu:DownloadManager::UbuntuDownloadManager udm READ udm
               NOTIFY udmChanged)
    Q_PROPERTY(int updatesCount READ updatesCount NOTIFY updatesCountChanged)

    const bool online();
    void setOnline(const bool &online);

    const bool authenticated();
    void setAuthenticated(const bool &authenticated);

    const Ubuntu:DownloadManager::UbuntuDownloadManager udm();

    const int updatesCount();

    enum UpdateMode {
        Downloadable,
        Installable,
        InstallableWithRestart,
        Pausable,
        NonPausable,
        Retriable
    };

    enum UpdateStatus {
        NotStarted,
        AutomaticallyDownloading,
        ManuallyDownloading,
        DownloadPaused,
        InstallationPaused,
        Installing,
        Installed,
        Failed
    };

signals:
    void onlineChanged();
    void authenticatedChanged();
    void udmChanged();
    void updatesCountChanged();

    // void requestClickUpdateMetadata();
    // void requestInstalledClicks();

private slots:
    void foundClickUpdate(QMap<QString, QVariant> clickUpdateMetadata);



private:
    static UpdateManager *m_instance;

    bool m_online;
    bool m_authenticated;
    bool m_updatesCount;

    ClickUpdateChecker m_clickUpChecker;
    SystemUpdate m_systemUpdate;
    Ubuntu:DownloadManager::UbuntuDownloadManager m_udm;
    UbuntuOne::SSOService m_ssoService;
};

}

#endif // UPDATEMANAGER_H
