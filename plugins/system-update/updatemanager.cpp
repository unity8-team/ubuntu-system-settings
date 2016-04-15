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
#include "helpers.h"

namespace UpdatePlugin {

UpdateManager *UpdateManager::m_instance = 0;

UpdateManager *UpdateManager::instance()
{
    qWarning() << "trace instance1";
    if (!m_instance)
        m_instance = new UpdateManager;

    qWarning() << "trace instance2";
    return m_instance;
}

UpdateManager::UpdateManager(QObject *parent):
    QObject(parent),
    m_online(false),
    m_authenticated(false),
    m_haveSufficientPower(false),
    m_updatesCount(-1),
    m_clickUpdatesCount(0),
    m_systemUpdatesCount(0),
    m_managerStatus(ManagerStatus::Idle),
    m_udm(nullptr),
    m_token(UbuntuOne::Token())
{
    qDBusRegisterMetaType<StringMap>();
    qWarning() << "trace a";
    initializeSystemImage();
    qWarning() << "trace b";
    initializeClickUpdateChecker();
    qWarning() << "trace c";
    initializeSSOService();
    qWarning() << "trace c9";
}

UpdateManager::~UpdateManager()
{
    qWarning() << "trace deletion1";
    if (m_udm)
        m_udm->deleteLater();
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
            SIGNAL(updateAvailable(
                    const QSharedPointer<ClickUpdateMetadata>&)),
            this,
            SLOT(onClickUpdateAvailable(
                    const QSharedPointer<ClickUpdateMetadata>&))
    );

    // If click update checker gets back 401/403, the credentials we had
    // were probably bad, so invalidate them locally via the
    // handleCredentialsFailed slot.
    connect(&m_clickUpChecker, SIGNAL(credentialError()),
            this, SLOT(handleCredentialsFailed())
    );
    connect(&m_clickUpChecker, SIGNAL(checkCompleted()),
            this, SLOT(onClickCheckCompleted())
    );
    connect(&m_clickUpChecker, SIGNAL(networkError()),
            this, SLOT(onNetworkError())
    );
    connect(&m_clickUpChecker, SIGNAL(serverError()),
            this, SLOT(onServerError())
    );
}


void UpdateManager::initializeSSOService()
{
    qWarning() << "trace c0";
    connect(&m_ssoService, SIGNAL(credentialsFound(const Token&)),
            this, SLOT(handleCredentialsFound(const Token&))
    );
    qWarning() << "trace c1";
    connect(&m_ssoService, SIGNAL(credentialsNotFound()),
            this, SLOT(handleCredentialsFailed())
    );
    qWarning() << "trace c2";
    connect(&m_ssoService, SIGNAL(credentialsDeleted()),
            this, SLOT(handleCredentialsFailed())
    );
    qWarning() << "trace c3";
}


void UpdateManager::initializeUdm()
{
    qWarning() << "trace d";
    // We're return m_udm to QML, so we need to explicitly retain ownership.
    QQmlEngine::setObjectOwnership(m_udm, QQmlEngine::CppOwnership);
    qWarning() << "trace e";
    m_udm->setParent(this);
    qWarning() << "trace f";

    connect(m_udm, SIGNAL(downloadsChanged()),
            this, SLOT(updateClickUpdatesCount()));
    qWarning() << "trace g";

    updateClickUpdatesCount();
    qWarning() << "trace h";
}

void UpdateManager::updateClickUpdatesCount()
{
    qWarning() << "trace 12";
    // m_clickUpdatesCount = m_udm->property("downloads").toList().count();
    qWarning() << "updating click updates count..." << m_clickUpdatesCount;
    calculateUpdatesCount();
}

bool UpdateManager::online() const
{
    return m_online;
}

void UpdateManager::setOnline(const bool online)
{
    qWarning() << "sat online" << online;
    if (online != m_online) {
        m_online = online;
        Q_EMIT onlineChanged();
    }

    if (m_online) {
        checkForUpdates();
    } else {
        cancelCheckForUpdates();
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
    qWarning() << "trace setudm1" << udm;
    if (m_udm != udm) {
        m_udm = udm;
        Q_EMIT udmChanged();
        qWarning() << "trace setudm2";
    }

    qWarning() << "trace setudm3";
    if (m_udm) {
        qWarning() << "trace setudm4";
        initializeUdm();
        qWarning() << "trace setudm5";
    }
}

int UpdateManager::updatesCount() const
{
    return m_updatesCount;
}

void UpdateManager::calculateUpdatesCount() {
    qWarning() << "trace 4";
    int newCount = m_clickUpdatesCount + m_systemUpdatesCount;
    if (newCount != m_updatesCount) {
        m_updatesCount = newCount;
        qWarning() << "trace 1";
        Q_EMIT updatesCountChanged();
        qWarning() << "trace 3";
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
        qWarning() << "checkForUpdates checking creds";
        m_ssoService.getCredentials();
    }

    m_systemImage.checkForUpdate();
    m_clickUpChecker.check();
    setManagerStatus(ManagerStatus::CheckingAllUpdates);
}

void UpdateManager::cancelCheckForUpdates()
{
    m_systemImage.cancelUpdate();
    m_clickUpChecker.cancel();
    setManagerStatus(ManagerStatus::Idle);
}

void UpdateManager::setManagerStatus(const UpdateManager::ManagerStatus &status)
{
    if (m_managerStatus != status) {
        m_managerStatus = status;
        Q_EMIT managerStatusChanged();
    }
    QString s;
    switch(m_managerStatus) {
    case Idle:
        s = "Idle"; break;
    case CheckingClickUpdates:
        s = "CheckingClickUpdates"; break;
    case CheckingSystemUpdates:
        s = "CheckingSystemUpdates"; break;
    case CheckingAllUpdates:
        s = "CheckingAllUpdates"; break;
    case NetworkError:
        s = "NetworkError"; break;
    case ServerError:
        s = "ServerError"; break;
    }
    qWarning() << "manager: status now" << s;
}

bool UpdateManager::clickUpdateInUdm(const QSharedPointer<ClickUpdateMetadata> &meta) const
{
    bool found = false;
    qWarning() << "manager: clickUpdateInUdm check on" << meta->name();
    foreach(const QVariant &v, m_udm->property("downloads").toList()) {
        QObject *download = v.value<QObject*>();
        qWarning() << download->property("metadata");
        // QObject *downloadMeta = download->property("metadata").value<QObject*>();
        // foreach(const QString &key, download->property(""))
        // qWarning() << v.value<QObject*>()->property("downloadId");
    }
    return found;
}

void UpdateManager::onClickUpdateAvailable(const QSharedPointer<ClickUpdateMetadata> &meta)
{
    qWarning() << "manager: found downloadable click update metadata" << meta->name();

    if (!clickUpdateInUdm(meta)) {
        qWarning() << "manager: click metadata was not in udm, downloading..";
        createClickUpdateDownload(meta);
    }
}

void UpdateManager::createClickUpdateDownload(const QSharedPointer<ClickUpdateMetadata> &meta)
{
    StringMap headers;
    headers[X_CLICK_TOKEN] = meta->clickToken();

    QStringList command;
    command << Helpers::whichPkcon() << "-p" << "install-local" << "$file";

    QVariantMap metadata;
    metadata["command"] = command;
    metadata["package-name"] = meta->name();
    metadata["title"] = meta->title();
    metadata["indicator-shown"] = false;
    qWarning() << "manager: create click download" << meta->downloadUrl() << meta->downloadSha512() << metadata << headers;
    Q_EMIT clickUpdateReady(meta->downloadUrl(), meta->downloadSha512(),
                            "sha512", metadata, headers);
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

    // Set click update checker's token, and start a check.
    m_clickUpChecker.setToken(token);
    m_clickUpChecker.check();
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
// we delegate that to SystemUpdate.qml.
void UpdateManager::handleSiAvailableStatus(const bool isAvailable,
                                            const bool downloading,
                                            const QString &availableVersion,
                                            const int &updateSize,
                                            const QString &lastUpdateDate,
                                            const QString &errorReason)
{
    qWarning() << "got available status from SI";

    m_systemUpdatesCount = isAvailable ? 1 : 0;
    calculateUpdatesCount();

    if (m_managerStatus == ManagerStatus::CheckingAllUpdates) {
        setManagerStatus(ManagerStatus::CheckingClickUpdates);
    } else if (m_managerStatus == ManagerStatus::CheckingSystemUpdates) {
        setManagerStatus(ManagerStatus::Idle);
    }
}

void UpdateManager::onClickCheckCompleted()
{
    qWarning() << "manager: click check completed";
    if (m_managerStatus == ManagerStatus::CheckingAllUpdates) {
        setManagerStatus(ManagerStatus::CheckingSystemUpdates);
    } else if (m_managerStatus == ManagerStatus::CheckingClickUpdates) {
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
