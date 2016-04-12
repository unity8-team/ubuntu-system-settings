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
*/

namespace UpdatePlugin {

UpdateManager *UpdateManager::m_instance = 0;

UpdateManager *UpdateManager::instance()
{
    if (!m_instance)
        m_instance = new UpdateManager;

    return m_instance;
}

UpdateManager::UpdateManager(QObject *parent):
    QObject(parent)
{
    setUpSystemImage();
    setUpClickUpdateChecker();
    setUpSSOService();
    setUpUdm();
}

UpdateManager::~UpdateManager()
{
}

void UpdateManager::setUpSystemImage()
{

    connect(&m_systemImage,
            SIGNAL(downloadModeChanged()),
            this,
            SIGNAL(downloadModeChanged())
    );
    connect(&m_systemImage,
            SIGNAL(updateAvailableStatus(const bool isAvailable,
                                         const bool downloading,
                                         const QString &availableVersion,
                                         const int &updateSize,
                                         const QString &lastUpdateDate,
                                         const QString &errorReason)),
            this,
            SLOT(handleSiAvailableStatus(const bool isAvailable,
                                         const bool downloading,
                                         const QString &availableVersion,
                                         const int &updateSize,
                                         const QString &lastUpdateDate,
                                         const QString &errorReason))
    );

}


void UpdateManager::setUpClickUpdateChecker()
{
    connect(&m_clickUpChecker,
            SIGNAL( clickUpdateDownloadable(
                    const ClickUpdateMetadata &clickUpdateMetadata)),
            this,
            SLOT(handleClickUpdateMetadata(
                    const ClickUpdateMetadata &clickUpdateMetadata))
    );

    // If click update checker gets back 401/403, the credentials we had
    // were probably bad, so invalidate them locally via the
    // handleCredentialsFailed slot.
    connect(&m_clickUpChecker,
            SIGNAL(credentialError()),
            this,
            SLOT(handleCredentialsFailed())
    );

    connect(&m_clickUpChecker,
            SIGNAL(checkCompleted()),
            this,
            SLOT(handleClickCheckCompleted())
    );
}


void UpdateManager::setUpSSOService()
{
    connect(&m_ssoService,
            SIGNAL(credentialsFound(const Token &token)),
            this,
            SLOT(handleCredentialsFound(const Token &token))
    );
    connect(&m_ssoService,
            SIGNAL(credentialsNotFound()),
            this,
            SLOT(handleCredentialsFailed())
    );
    connect(&m_ssoService,
            SIGNAL(credentialsDeleted()),
            this,
            SLOT(handleCredentialsFailed())
    );
}


void UpdateManager::setUpUdm()
{

}


bool UpdateManager::online() const
{
    return m_online;
}

void UpdateManager::setOnline(const bool &online)
{
    if (online != m_online) {
        m_online = online;
        Q_EMIT onlineChanged();
    }
}

bool UpdateManager::authenticated() const
{
    return m_authenticated;
}

void UpdateManager::setAuthenticated(const bool &authenticated)
{
    if (authenticated != m_authenticated) {
        m_authenticated = authenticated;
        Q_EMIT authenticatedChanged();
    }

}

bool UpdateManager::haveSufficientPower() const
{
    return m_haveSufficientPower;
}

void UpdateManager::setHaveSufficientPower(const bool &haveSufficientPower)
{
    if (haveSufficientPower != m_haveSufficientPower) {
        m_haveSufficientPower = haveSufficientPower;
        Q_EMIT authenticatedChanged();
    }

}

Ubuntu:DownloadManager::UbuntuDownloadManager UpdateManager::udm() const
{
    return m_udm;
}


int UpdateManager::updatesCount() const
{
    return m_updatesCount;
}

void UpdateManager::setUpdatesCount(const int &count) {
    if (count != m_updatesCount) {
        m_updatesCount = count;
        Q_EMIT updatesCountChanged();
    }
}

int UpdateManager::downloadMode() const
{
    return m_systemImage.downloadMode();
}

void UpdateManager::setDownloadMode(const int &downloadMode)
{
    m_systemImage.setDownloadMode(downloadMode);
}

ManagerStatus UpdateManager::managerStatus() const
{
    return m_managerStatus;
}

void UpdateManager::checkForUpdates()
{
    if (!m_token) {
        m_ssoService.getCredentials();
    }

    m_systemImage.checkForUpdates();
    m_clickUpChecker.checkForUpdates();
    setManagerStatus(ManagerStatus::CheckingAllUpdates);
}

void UpdateManager::abortCheckForUpdates()
{
    // TODO: Figure out way to cancel SI check
    m_clickUpChecker.abortCheckForUpdates();
    setManagerStatus(ManagerStatus::Idle);
}

void UpdateManager::setManagerStatus(const ManagerStatus &status)
{
    if (m_managerStatus != status) {
        m_managerStatus = status;
        Q_EMIT managerStatusChanged();
    }
}

bool UpdateManager::clickUpdateInUdm(const ClickUpdateMetadata &clickUpdateMetadata) const
{

}

void UpdateManager::createClickUpdateDownload(const ClickUpdateMetadata &clickUpdateMetadata)
{

}

void UpdateManager::handleCredentialsFound(const Token &token)
{
    m_token = token;

    if (!m_token.isValid()) {
        qWarning() << "updateManager got invalid token.";
        handleCredentialsFailed();
        return;
    }

    // Set click update checker's token, and start a check.
    m_clickUpChecker.setToken(token);
    m_clickUpChecker.checkForUpdates();
}

void UpdateManager::handleCredentialsFailed()
{
    m_ssoService.invalidateCredentials();
    m_token = null;

    // Ask click update checker to stop checking for updates.
    // Revoke the token given to click update checker.
    m_clickUpChecker.abortCheckForUpdates();
    m_clickUpChecker.setToken(null);

    // We've invalidated the token, and the user is now not authenticated.
    setAuthenticated(false);
}

// We don't handle the contents of the available status here,
// we delegate that to SystemUpdate.qml.
void UpdateManager::handleSiAvailableStatus(const bool isAvailable,
                                            const bool downloading,
                                            const QString &availableVersion,
                                            const int &updateSize,
                                            const QString &lastUpdateDate,
                                            const QString &errorReason)
{
    if (m_managerStatus == ManagerStatus::CheckingAllUpdates) {
        setManagerStatus(ManagerStatus::CheckingClickUpdates);
    } else if (m_managerStatus == ManagerStatus::CheckingSystemUpdates) {
        setManagerStatus(ManagerStatus::Idle);
    }
}

void UpdateManager::handleClickCheckCompleted()
{
    if (m_managerStatus == ManagerStatus::CheckingAllUpdates) {
        setManagerStatus(ManagerStatus::CheckingSystemUpdates);
    } else if (m_managerStatus == ManagerStatus::CheckingClickUpdates) {
        setManagerStatus(ManagerStatus::Idle);
    }
}

} // UpdatePlugin
