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
#include "updatemanager.h"

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlError>

namespace UpdatePlugin {

UpdateManager *UpdateManager::m_instance = 0;

UpdateManager *UpdateManager::instance()
{
    if (!m_instance)
        m_instance = new UpdateManager;

    return m_instance;
}

UpdateManager::UpdateManager(QObject *parent):
    QObject(parent),
    m_updatesCount(-1)
{
    initializeSystemImage();
    initializeClickUpdateChecker();
    initializeSSOService();
}

UpdateManager::~UpdateManager()
{
}

void UpdateManager::initializeSystemImage()
{

    connect(&m_systemImage,
            SIGNAL(downloadModeChanged()),
            this,
            SIGNAL(downloadModeChanged())
    );
    connect(&m_systemImage,
            SIGNAL(updateAvailableStatus(const bool,
                                         const bool,
                                         const QString&,
                                         const int&,
                                         const QString&,
                                         const QString&)),
            this,
            SLOT(handleSiAvailableStatus(const bool,
                                         const bool,
                                         const QString&,
                                         const int&,
                                         const QString&,
                                         const QString&))
    );
}


void UpdateManager::initializeClickUpdateChecker()
{
    connect(&m_clickUpChecker,
            SIGNAL(clickUpdateDownloadable(
                    const QSharedPointer<ClickUpdateMetadata>&)),
            this,
            SLOT(downloadClickUpdate(
                    const QSharedPointer<ClickUpdateMetadata>&))
    );

    // If click update checker gets back 401/403, the credentials we had
    // were probably bad, so invalidate them locally via the
    // handleCredentialsFailed slot.
    connect(&m_clickUpChecker, SIGNAL(credentialError()),
            this, SLOT(handleCredentialsFailed())
    );

    connect(&m_clickUpChecker, SIGNAL(checkCompleted()),
            this, SLOT(handleClickCheckCompleted())
    );
}


void UpdateManager::initializeSSOService()
{
    connect(&m_ssoService, SIGNAL(credentialsFound(const Token&)),
            this, SLOT(handleCredentialsFound(const Token&))
    );
    connect(&m_ssoService, SIGNAL(credentialsNotFound()),
            this, SLOT(handleCredentialsFailed())
    );
    connect(&m_ssoService, SIGNAL(credentialsDeleted()),
            this, SLOT(handleCredentialsFailed())
    );
}


void UpdateManager::initializeUdm()
{
    updateClickUpdatesCount();
    connect(m_udm, SIGNAL(downloadsChanged()),
            this, SLOT(updateClickUpdatesCount()));
}

void UpdateManager::updateClickUpdatesCount()
{
    m_clickUpdatesCount = m_udm->property("downloads").toList().count();
    qWarning() << "updating click updates count..." << m_clickUpdatesCount;
    setUpdatesCount();
}

bool UpdateManager::online() const
{
    return m_online;
}

void UpdateManager::setOnline(const bool online)
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

void UpdateManager::setAuthenticated(const bool authenticated)
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

void UpdateManager::setHaveSufficientPower(const bool haveSufficientPower)
{
    if (haveSufficientPower != m_haveSufficientPower) {
        m_haveSufficientPower = haveSufficientPower;
        Q_EMIT authenticatedChanged();
    }
}

QObject *UpdateManager::udm() const
{
    return m_udm;
}

void UpdateManager::setUdm(QObject *udm) {
    qWarning() << "setUdm" << udm;
    if (m_udm != udm) {
        m_udm = udm;
        Q_EMIT udmChanged();
    }

    if (udm)
        initializeUdm();
}

int UpdateManager::updatesCount() const
{
    return m_updatesCount;
}

void UpdateManager::setUpdatesCount() {
    int newCount = m_clickUpdatesCount + m_systemUpdatesCount;
    if (newCount != m_updatesCount) {
        m_updatesCount = newCount;
        Q_EMIT updatesCountChanged();
    }
}

int UpdateManager::downloadMode()
{
    return m_systemImage.downloadMode();
}

void UpdateManager::setDownloadMode(const int &downloadMode)
{
    m_systemImage.setDownloadMode(downloadMode);
}

UpdateManager::ManagerStatus UpdateManager::managerStatus() const
{
    return m_managerStatus;
}

void UpdateManager::checkForUpdates()
{
    if (!m_token.isValid()) {
        m_ssoService.getCredentials();
    }

    m_systemImage.checkForUpdate();
    m_clickUpChecker.checkForUpdates();
    setManagerStatus(UpdateManager::ManagerStatus::ManagerCheckingAllUpdates);
}

void UpdateManager::abortCheckForUpdates()
{
    // TODO: Figure out way to cancel SI check
    m_clickUpChecker.abortCheckForUpdates();
    setManagerStatus(ManagerStatus::ManagerIdle);
}

void UpdateManager::setManagerStatus(const UpdateManager::ManagerStatus &status)
{
    if (m_managerStatus != status) {
        m_managerStatus = status;
        Q_EMIT managerStatusChanged();
    }
}

bool UpdateManager::clickUpdateInUdm(const QSharedPointer<ClickUpdateMetadata> &meta) const
{

}

void UpdateManager::downloadClickUpdate(const QSharedPointer<ClickUpdateMetadata> &meta)
{

}

void UpdateManager::createClickUpdateDownload(const QSharedPointer<ClickUpdateMetadata> &meta)
{

}

void UpdateManager::handleCredentialsFound(const UbuntuOne::Token &token)
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
    m_token = UbuntuOne::Token();

    // Ask click update checker to stop checking for updates.
    // Revoke the token given to click update checker.
    m_clickUpChecker.abortCheckForUpdates();
    m_clickUpChecker.setToken(m_token);

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
    if (m_managerStatus == UpdateManager::ManagerStatus::ManagerCheckingAllUpdates) {
        setManagerStatus(UpdateManager::ManagerStatus::ManagerCheckingClickUpdates);
    } else if (m_managerStatus == UpdateManager::ManagerStatus::ManagerCheckingSystemUpdates) {
        setManagerStatus(UpdateManager::ManagerStatus::ManagerIdle);
    }
}

void UpdateManager::handleClickCheckCompleted()
{
    if (m_managerStatus == UpdateManager::ManagerStatus::ManagerCheckingAllUpdates) {
        setManagerStatus(UpdateManager::ManagerStatus::ManagerCheckingSystemUpdates);
    } else if (m_managerStatus == UpdateManager::ManagerStatus::ManagerCheckingClickUpdates) {
        setManagerStatus(UpdateManager::ManagerStatus::ManagerIdle);
    }
}

} // UpdatePlugin
