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
#include "helpers.h"

namespace UpdatePlugin
{

UpdateManager *UpdateManager::m_instance = 0;

UpdateManager *UpdateManager::instance()
{
    if (!m_instance) m_instance = new UpdateManager;

    return m_instance;
}

UpdateManager::UpdateManager(QObject *parent) :
        QObject(parent),
        m_online(false),
        m_authenticated(false),
        m_haveSufficientPower(false),
        m_updatesCount(-1),
        m_clickUpdatesCount(0),
        m_systemUpdatesCount(0),
        m_managerStatus(ManagerStatus::CheckingAllUpdates),
        m_systemImage(nullptr),
        m_token(UbuntuOne::Token()),
        m_updatestore(this)
{
    connect(this, SIGNAL(connected()), this, SLOT(checkForUpdates()));
    connect(this, SIGNAL(disconnected()), this, SLOT(cancelCheckForUpdates()));

    initializeSystemImage();
    initializeClickUpdateChecker();
    initializeSSOService();
    initializeUpdateStore();

    calculateUpdatesCount();
}

UpdateManager::~UpdateManager()
{
}

void UpdateManager::initializeSystemImage()
{
    if (m_systemImage == nullptr) m_systemImage = new QSystemImage(this);

    connect(m_systemImage, SIGNAL(downloadModeChanged()), this,
            SIGNAL(downloadModeChanged()));
    connect(m_systemImage,
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
                                         const QString&)));

    if (m_systemImage->getIsTargetNewer()) {
        m_systemUpdatesCount = 1;
        calculateUpdatesCount();
    }
}

void UpdateManager::initializeClickUpdateChecker()
{
    connect(&m_clickUpChecker,
            SIGNAL(updateAvailable(const ClickUpdateMetadata*)),
            this,
            SLOT(onClickUpdateAvailable(const ClickUpdateMetadata*)));

    // If click update checker gets back 401/403, the credentials we had
    // were probably bad, so invalidate them locally via the
    // handleCredentialsFailed slot.
    connect(&m_clickUpChecker, SIGNAL(credentialError()), this,
            SLOT(handleCredentialsFailed()));
    connect(&m_clickUpChecker, SIGNAL(checkCompleted()), this,
            SLOT(onClickCheckCompleted()));
    connect(&m_clickUpChecker, SIGNAL(networkError()), this,
            SLOT(onNetworkError()));
    connect(&m_clickUpChecker, SIGNAL(serverError()), this,
            SLOT(onServerError()));
}

void UpdateManager::initializeSSOService()
{
    connect(&m_ssoService, SIGNAL(credentialsFound(const Token&)), this,
            SLOT(handleCredentialsFound(const Token&)));
    connect(&m_ssoService, SIGNAL(credentialsNotFound()), this,
            SLOT(handleCredentialsFailed()));
    connect(&m_ssoService, SIGNAL(credentialsDeleted()), this,
            SLOT(handleCredentialsFailed()));
}

void UpdateManager::initializeUpdateStore()
{
    qWarning() << "last click check" << m_updatestore.lastCheckDate().toString("dd.MM.yyyy hh:mm:ss");

    m_clickUpdatesCount = m_updatestore.pendingClickUpdates()->rowCount();
    // qWarning()
    //         << "in initializeUpdateStore we set m_clickUpdatesCount to "
    //         << m_clickUpdatesCount;
    calculateUpdatesCount();
}

bool UpdateManager::online() const
{
    return m_online;
}

void UpdateManager::setOnline(const bool online)
{
    if (online != m_online) {
        qWarning() << "manager: online change:" << online;
        m_online = online;
        Q_EMIT onlineChanged();

        if (online) Q_EMIT connected();
        if (!online) Q_EMIT disconnected();
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

bool UpdateManager::haveSystemUpdate() const
{
    return m_systemUpdatesCount > 0;
}

QSystemImage* UpdateManager::systemImageBackend() const
{
    return m_systemImage;
}

void UpdateManager::setHaveSufficientPower(const bool haveSufficientPower)
{
    if (haveSufficientPower != m_haveSufficientPower) {
        m_haveSufficientPower = haveSufficientPower;
        Q_EMIT authenticatedChanged();
    }
}

int UpdateManager::updatesCount() const
{
    return m_updatesCount;
}

void UpdateManager::calculateUpdatesCount()
{
    int newCount = m_clickUpdatesCount + m_systemUpdatesCount;
    if (newCount != m_updatesCount) {
        m_updatesCount = newCount;
        Q_EMIT updatesCountChanged();
    }
}

int UpdateManager::downloadMode()
{
    return m_systemImage->downloadMode();
}

void UpdateManager::setDownloadMode(const int &downloadMode)
{
    m_systemImage->setDownloadMode(downloadMode);
}

UpdateManager::ManagerStatus UpdateManager::managerStatus() const
{
    return m_managerStatus;
}

UpdateModel *UpdateManager::installedUpdates()
{
    return m_updatestore.installedUpdates();
}

UpdateModel *UpdateManager::activeClickUpdates()
{
    return m_updatestore.pendingClickUpdates();
}

void UpdateManager::checkForUpdates()
{
    qWarning() << "manager: check";
    m_systemImage->checkForUpdate();
    setManagerStatus(ManagerStatus::CheckingSystemUpdates);

    // Don't check for click updates if there are no credentials.
    if (!m_token.isValid()) {
        m_ssoService.getCredentials();
        return;
    }

    // Start a click update check only if none in progress.
    if (managerStatus() == ManagerStatus::Idle
            || managerStatus() == ManagerStatus::CheckingSystemUpdates) {
        setManagerStatus(ManagerStatus::CheckingAllUpdates);
        m_clickUpChecker.check();
    }
}

void UpdateManager::cancelCheckForUpdates()
{
    m_clickUpChecker.cancel();
    setManagerStatus(ManagerStatus::Idle);
}

void UpdateManager::setManagerStatus(const ManagerStatus &status)
{
    if (m_managerStatus != status) {
        m_managerStatus = status;
        Q_EMIT managerStatusChanged();
    }
    // QString s;
    // switch(m_managerStatus) {
    // case ManagerStatus::Idle:
    //     s = "Idle"; break;
    // case ManagerStatus::CheckingClickUpdates:
    //     s = "CheckingClickUpdates"; break;
    // case ManagerStatus::CheckingSystemUpdates:
    //     s = "CheckingSystemUpdates"; break;
    // case ManagerStatus::CheckingAllUpdates:
    //     s = "CheckingAllUpdates"; break;
    // case ManagerStatus::BatchMode:
    //     s = "BatchMode"; break;
    // case ManagerStatus::NetworkError:
    //     s = "NetworkError"; break;
    // case ManagerStatus::ServerError:
    //     s = "ServerError"; break;
    // }
    // qWarning() << "manager: status now" << s;
}

void UpdateManager::onClickUpdateAvailable(const ClickUpdateMetadata *meta)
{
    qWarning() << "manager: found downloadable click update metadata"
            << meta->name();
    m_updatestore.add(meta);
}

void UpdateManager::retryClickPackage(const QString &packageName, const int &revision)
{
    Q_UNUSED(revision)
    m_clickUpChecker.check(packageName);
}

void UpdateManager::handleCredentialsFound(const UbuntuOne::Token &token)
{
    qWarning() << "found credentials";
    m_token = token;

    if (!m_token.isValid()) {
        qWarning() << "updateManager got invalid token.";
        handleCredentialsFailed();
        return;
    }

    setAuthenticated(true);

    // Set click update checker's token, cancel and start a new check.
    m_clickUpChecker.setToken(token);
    m_clickUpChecker.cancel();
    checkForUpdates();
}

void UpdateManager::handleCredentialsFailed()
{
    qWarning() << "failed credentials";
    m_ssoService.invalidateCredentials();
    m_token = UbuntuOne::Token();

    // Ask click update checker to stop checking for updates.
    // Revoke the token given to click update checker.
    m_clickUpChecker.cancel();
    m_clickUpChecker.setToken(m_token);

    // We've invalidated the token, and the user is now not authenticated.
    setAuthenticated(false);
}

// We don't handle the contents of the available status here,
// we delegate that to ImageUpdate.qml.
void UpdateManager::handleSiAvailableStatus(const bool isAvailable,
                                            const bool downloading,
                                            const QString &availableVersion,
                                            const int &updateSize,
                                            const QString &lastUpdateDate,
                                            const QString &errorReason)
{
    qWarning() << "got available status from SI";

    m_systemUpdatesCount = isAvailable ? 1 : 0;
    haveSystemUpdateChanged();
    calculateUpdatesCount();

    if (m_managerStatus == ManagerStatus::CheckingAllUpdates) {
        setManagerStatus(ManagerStatus::CheckingClickUpdates);
    } else if (m_managerStatus == ManagerStatus::CheckingSystemUpdates) {
        setManagerStatus(ManagerStatus::Idle);
    }
}

void UpdateManager::udmDownloadEnded(const QString &id)
{
    m_updatestore.unsetUdmId(id.toInt());
}

void UpdateManager::onClickCheckCompleted()
{
    qWarning() << "manager: click check completed";
    QString s;
    switch (m_managerStatus) {
    case ManagerStatus::Idle:
        s = "Idle";
        break;
    case ManagerStatus::CheckingClickUpdates:
        s = "CheckingClickUpdates";
        break;
    case ManagerStatus::CheckingSystemUpdates:
        s = "CheckingSystemUpdates";
        break;
    case ManagerStatus::CheckingAllUpdates:
        s = "CheckingAllUpdates";
        break;
    case ManagerStatus::BatchMode:
        s = "BatchMode";
        break;
    case ManagerStatus::NetworkError:
        s = "NetworkError";
        break;
    case ManagerStatus::ServerError:
        s = "ServerError";
        break;
    }
    qWarning() << "manager: click check completed status:" << s;

    if (m_managerStatus == ManagerStatus::CheckingAllUpdates) {
        qWarning() << "manager: wanted to set to CheckingSystemUpdates";
        setManagerStatus(ManagerStatus::CheckingSystemUpdates);
    } else if (m_managerStatus == ManagerStatus::CheckingClickUpdates) {
        qWarning() << "manager: wanted to set to ClickUpdates";
        setManagerStatus(ManagerStatus::Idle);
    }
}

void UpdateManager::onNetworkError()
{
    setManagerStatus(ManagerStatus::NetworkError);
}

void UpdateManager::onServerError()
{
    setManagerStatus(ManagerStatus::ServerError);
}

} // UpdatePlugin
