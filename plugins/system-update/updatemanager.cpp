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
        m_token(UbuntuOne::Token()),
        m_updatestore(this),
        m_authenticated(false)
{
    initializeClickUpdateChecker();
    initializeSSOService();
    initializeUpdateStore();
}

UpdateManager::~UpdateManager()
{
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
            SIGNAL(clickUpdateCheckCompleted()));
    connect(&m_clickUpChecker, SIGNAL(checkStarted()), this,
            SIGNAL(clickUpdateCheckStarted()));
    connect(&m_clickUpChecker, SIGNAL(networkError()), this,
            SIGNAL(networkError()));
    connect(&m_clickUpChecker, SIGNAL(serverError()), this,
            SIGNAL(serverError()));
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


    // qWarning()
    //         << "in initializeUpdateStore we set m_clickUpdatesCount to "
    //         << m_clickUpdatesCount;
    // calculateUpdatesCount();
}

bool UpdateManager::authenticated()
{
    return m_authenticated || Helpers::isIgnoringCredentials();
}

void UpdateManager::setAuthenticated(const bool authenticated)
{
    if (authenticated != m_authenticated) {
        m_authenticated = authenticated;
        Q_EMIT (authenticatedChanged());
    }
}

UpdateStore *UpdateManager::updateStore()
{
    return &m_updatestore;
}

void UpdateManager::checkForClickUpdates()
{
    qWarning() << "manager: check";

    // Don't check for click updates if there are no credentials,
    // instead ask for credentials.
    if (!m_token.isValid() && !Helpers::isIgnoringCredentials()) {
        m_ssoService.getCredentials();
        return;
    }
    m_clickUpChecker.check();

    // // Start a click update check only if none in progress.
    // if (managerStatus() == ManagerStatus::Idle
    //         || managerStatus() == ManagerStatus::CheckingSystemUpdates) {
    //     setManagerStatus(ManagerStatus::CheckingAllUpdates);
    // }
}

void UpdateManager::cancelCheckForClickUpdates()
{
    m_clickUpChecker.cancel();
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
    checkForClickUpdates();
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

void UpdateManager::clickUpdateInstalled(const QString &packageName, const int &revision)
{
    m_updatestore.markInstalled(packageName, revision);
}

} // UpdatePlugin
