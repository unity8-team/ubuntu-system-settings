/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Authors:
 * Diego Sarmentero <diego.sarmentero@canonical.com>
 *
*/

#include "update_manager.h"
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QProcessEnvironment>
#include <QDBusInterface>

#define CLICK_COMMAND "click"
#ifdef TESTS
    #define CHECK_CREDENTIALS "IGNORE_CREDENTIALS"
#else
    #define CHECK_CREDENTIALS "CHECK_CREDENTIALS"
#endif

namespace UpdatePlugin {

UpdateManager::UpdateManager(QObject *parent):
    QObject(parent),
    m_systemCheckingUpdate(false),
    m_clickCheckingUpdate(false),
    m_checkingUpdates(0),
    m_downloadMode(-1)
{
    // SSO SERVICE
    QObject::connect(&m_service, SIGNAL(credentialsFound(const Token&)),
                     this, SLOT(handleCredentialsFound(Token)));
    QObject::connect(&m_service, SIGNAL(credentialsNotFound()),
                     this, SIGNAL(credentialsNotFound()));
    QObject::connect(&m_service, SIGNAL(credentialsNotFound()),
                  this, SLOT(clickUpdateNotAvailable()));
    // PROCESS
    QObject::connect(&m_process, SIGNAL(finished(int)),
                  this, SLOT(processOutput()));
    // NETWORK
    QObject::connect(&m_network, SIGNAL(updatesFound()),
                  this, SLOT(processUpdates()));
    QObject::connect(&m_network, SIGNAL(updatesNotFound()),
                  this, SLOT(clickUpdateNotAvailable()));
    QObject::connect(&m_network, SIGNAL(errorOccurred()),
                  this, SIGNAL(errorFound()));
    QObject::connect(&m_network,
                     SIGNAL(clickTokenObtained(Update*, const QString&)),
                     this, SLOT(clickTokenReceived(Update*, const QString&)));
    QObject::connect(&m_network,
                     SIGNAL(downloadUrlFound(const QString&, const QString&)),
                     this, SLOT(downloadUrlObtained(const QString&, const QString&)));
    // SYSTEM UPDATE
    QObject::connect(&m_systemUpdate, SIGNAL(updateAvailable(const QString&, Update*)),
                  this, SLOT(registerSystemUpdate(const QString&, Update*)));
    QObject::connect(&m_systemUpdate, SIGNAL(updateNotFound()),
                  this, SLOT(systemUpdateNotAvailable()));
    QObject::connect(&m_systemUpdate, SIGNAL(downloadModeChanged()),
                  SIGNAL(downloadModeChanged()));
    QObject::connect(&m_systemUpdate, SIGNAL(updateDownloaded()),
                  SIGNAL(systemUpdateDownloaded()));
    QObject::connect(&m_systemUpdate, SIGNAL(updateProcessFailed(const QString&)),
                  SIGNAL(updateProcessFailed(QString)));
    QObject::connect(&m_systemUpdate, SIGNAL(updateFailed(int, QString)),
                  SIGNAL(systemUpdateFailed(int, QString)));
    QObject::connect(&m_systemUpdate, SIGNAL(updatePaused(int)),
                  SLOT(systemUpdatePaused(int)));
}

UpdateManager::~UpdateManager()
{
}

void UpdateManager::clickUpdateNotAvailable()
{
    m_clickCheckingUpdate = false;
    reportCheckState();
    updateNotAvailable();
}

void UpdateManager::systemUpdateNotAvailable()
{
    m_systemCheckingUpdate = false;
    reportCheckState();
    updateNotAvailable();
}

void UpdateManager::updateNotAvailable()
{
    m_checkingUpdates--;
    if (m_checkingUpdates == 0 && m_model.count() == 0) {
        Q_EMIT updatesNotFound();
    }
}

void UpdateManager::reportCheckState()
{
    if (!m_clickCheckingUpdate && !m_systemCheckingUpdate) {
        Q_EMIT checkFinished();
    }
}

void UpdateManager::checkUpdates()
{
    m_systemCheckingUpdate = true;
    m_clickCheckingUpdate = true;
    m_checkingUpdates = 2;
    m_model.clear();
    m_apps.clear();
    Q_EMIT modelChanged();
    bool enabled = enableAutopilotMode();
    if (getCheckForCredentials()) {
        m_systemUpdate.checkForUpdate();
        m_service.getCredentials();
    } else if (enabled) {
        systemUpdateNotAvailable();
        Token token("", "", "", "");
        handleCredentialsFound(token);
    } else {
        systemUpdateNotAvailable();
        clickUpdateNotAvailable();
    }
}

void UpdateManager::handleCredentialsFound(Token token)
{
    m_token = token;
    QStringList args("list");
    args << "--manifest";
    QString command = getClickCommand();
    m_process.start(command, args);
}

QString UpdateManager::getClickCommand()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("CLICK_COMMAND", QString(CLICK_COMMAND));
    return command;
}

bool UpdateManager::getCheckForCredentials()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString value = environment.value("IGNORE_CREDENTIALS", QString(CHECK_CREDENTIALS));
    return value == "CHECK_CREDENTIALS";
}

bool UpdateManager::enableAutopilotMode()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString value = environment.value("AUTOPILOT_ENABLED", QString("AUTOPILOT_DISABLED"));
    return value == "AUTOPILOT_ENABLED";
}

void UpdateManager::processOutput()
{
    QString output(m_process.readAllStandardOutput());

    QJsonDocument document = QJsonDocument::fromJson(output.toUtf8());

    QJsonArray array = document.array();

    int i;
    for (i = 0; i < array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        QString name = object.value("name").toString();
        QString title = object.value("title").toString();
        QString version = object.value("version").toString();
        Update *app = new Update();
        app->initializeApplication(name, title, version);
        m_apps[app->getPackageName()] = app;
    }

    m_network.checkForNewVersions(m_apps);
}

void UpdateManager::processUpdates()
{
    m_clickCheckingUpdate = false;
    bool updateAvailable = false;
    foreach (QString id, m_apps.keys()) {
        Update *app = m_apps.value(id);
        QString packagename(UBUNTU_PACKAGE_NAME);
        if(app->getPackageName() != packagename && app->updateRequired()) {
            updateAvailable = true;
            m_model.append(QVariant::fromValue(app));
        }
    }

    if (updateAvailable) {
        Q_EMIT modelChanged();
        Q_EMIT updateAvailableFound(false);
    }
    reportCheckState();
}

void UpdateManager::registerSystemUpdate(const QString& packageName, Update *update)
{
    QString packagename(UBUNTU_PACKAGE_NAME);
    if (!m_apps.contains(packagename)) {
        m_apps[packageName] = update;
        m_model.insert(0, QVariant::fromValue(update));
        Q_EMIT modelChanged();
        Q_EMIT updateAvailableFound(update->updateState());
    }
    m_systemCheckingUpdate = false;

    reportCheckState();
}

void UpdateManager::systemUpdatePaused(int value)
{
    QString packagename(UBUNTU_PACKAGE_NAME);
    if (m_apps.contains(packagename)) {
        Update *update = m_apps[packagename];
        update->setSelected(true);
        update->setDownloadProgress(value);
    }
}

void UpdateManager::startDownload(const QString &packagename)
{
    m_apps[packagename]->setUpdateState(true);
    if (m_apps[packagename]->systemUpdate()) {
        m_systemUpdate.downloadUpdate();
    } else {
        m_network.getResourceUrl(packagename);
    }
}

void UpdateManager::retryDownload(const QString &packagename)
{
    if (m_apps[packagename]->systemUpdate()) {
        Update *update = m_apps.take(packagename);
        m_systemUpdate.cancelUpdate();
        m_model.removeAt(0);
        update->deleteLater();
        Q_EMIT modelChanged();
        m_systemUpdate.checkForUpdate();
    } else {
        startDownload(packagename);
    }
}

void UpdateManager::pauseDownload(const QString &packagename)
{
    m_apps[packagename]->setUpdateState(false);
    m_systemUpdate.pauseDownload();
}

void UpdateManager::downloadUrlObtained(const QString &packagename,
                                        const QString &url)
{
    if (m_token.isValid()) {
        QString authHeader = m_token.signUrl(url, QStringLiteral("HEAD"), true);
        Update *app = m_apps[packagename];
        app->setClickUrl(url);
        m_network.getClickToken(app, url, authHeader);
    } else {
        Update *app = m_apps[packagename];
        app->setError("Invalid User Token");
    }
}

void UpdateManager::clickTokenReceived(Update *app, const QString &clickToken)
{
    app->setError("");
    app->setClickToken(clickToken);
    app->setDownloadUrl(app->getClickUrl());
}

void UpdateManager::updateClickScope()
{
    // Refresh click scope
    QDBusInterface iface("com.canonical.unity.scopes",
                         "/com/canonical/unity/scopes",
                         "com.canonical.unity.scopes",
                         QDBusConnection::sessionBus(), 0);
    iface.call(QLatin1String("InvalidateResults"), QLatin1String("clickscope"));
}

}
