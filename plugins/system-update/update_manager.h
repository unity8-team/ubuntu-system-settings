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

#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QtQml>
#include <QDateTime>
#include <QHash>
#include <QList>
#include <QVariant>
#include <QVariantList>
#include "system_update.h"
#include "update.h"
#include <token.h>

#ifdef TESTS
#include "../../tests/plugins/system-update/fakeprocess.h"
#include "../../tests/plugins/system-update/fakenetwork.h"
#include "../../tests/plugins/system-update/fakessoservice.h"
#include "../../tests/plugins/system-update/fakesystemupdate.h"
#else
#include <ssoservice.h>
#include <QProcess>
#include "network/network.h"
#include "system_update.h"
#endif

// Having the full namespaced name in a slot seems to confuse
// SignalSpy so we need this declaration.
using UbuntuOne::Token;

namespace UpdatePlugin {

class UpdateManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList model READ model NOTIFY modelChanged)
    Q_PROPERTY(int downloadMode READ downloadMode WRITE setDownloadMode
               NOTIFY downloadModeChanged)
    Q_PROPERTY(int currentBuildNumber READ currentBuildNumber CONSTANT)
    Q_PROPERTY(QDateTime lastUpdateDate READ lastUpdateDate CONSTANT)
    Q_PROPERTY(QString currentUbuntuBuildNumber READ currentUbuntuBuildNumber
               NOTIFY versionChanged)
    Q_PROPERTY(QString currentDeviceBuildNumber READ currentDeviceBuildNumber
               NOTIFY versionChanged)

Q_SIGNALS:
    void checkFinished();
    void modelChanged();
    void updatesNotFound();
    void credentialsNotFound();
    void updateAvailableFound(bool downloading);
    void errorFound();
    void downloadModeChanged();
    void systemUpdateDownloaded();
    void updateProcessFailed(QString message);
    void systemUpdateFailed(int consecutiveFailureCount, QString lastReason);
    void versionChanged();
    void rebooting(bool status);
    
public:
    explicit UpdateManager(QObject *parent = 0);
    ~UpdateManager();

    Q_INVOKABLE void checkUpdates();
    Q_INVOKABLE void startDownload(const QString &packagename);
    Q_INVOKABLE void pauseDownload(const QString &packagename);
    Q_INVOKABLE void retryDownload(const QString &packagename);
    Q_INVOKABLE void applySystemUpdate() { m_systemUpdate.applyUpdate(); }
    Q_INVOKABLE void updateClickScope();

    QVariantList model() const { return m_model; }
    int downloadMode() { return m_systemUpdate.downloadMode(); }
    void setDownloadMode(int mode) { m_systemUpdate.setDownloadMode(mode); }
    int currentBuildNumber() { return m_systemUpdate.currentBuildNumber(); }
    QDateTime lastUpdateDate() { return m_systemUpdate.lastUpdateDate(); }
    QString currentUbuntuBuildNumber() { return m_systemUpdate.currentUbuntuBuildNumber(); }
    QString currentDeviceBuildNumber() { return m_systemUpdate.currentDeviceBuildNumber(); }

#ifdef TESTS
    // For testing purposes
    QHash<QString, Update*> get_apps() { return m_apps; }
    QVariantList get_model() { return m_model; }
    int get_downloadMode() { return m_downloadMode; }
    void set_token(Token& t) { m_token = t; }
    Token get_token() { return m_token; }
    void setCheckintUpdates(int value) { m_checkingUpdates = value; }
    void setCheckSystemUpdates(int value) { m_systemCheckingUpdate = value; }
    void setCheckClickUpdates(int value) { m_clickCheckingUpdate = value; }
    FakeSsoService& getService() { return m_service; }
#endif

public Q_SLOTS:
    void registerSystemUpdate(const QString& packageName, Update *update);
    void systemUpdateNotAvailable();

private Q_SLOTS:
    void clickUpdateNotAvailable();
    void updateFailed(int consecutiveFailureCount, QString lastReason);
    void updateDownloaded();
    void systemUpdatePaused(int value);
    void systemUpdateProgress(int value, double eta);
    void processOutput();
    void processUpdates();
    void downloadUrlObtained(const QString &packagename, const QString &url);
    void handleCredentialsFound(Token token);
    void clickTokenReceived(Update *app, const QString &clickToken);

private:
    bool m_systemCheckingUpdate;
    bool m_clickCheckingUpdate;
    int m_checkingUpdates;
    QHash<QString, Update*> m_apps;
    int m_downloadMode;
    QVariantList m_model;
    Token m_token;

#ifdef TESTS
    FakeNetwork m_network;
    FakeProcess m_process;
    FakeSsoService m_service;
    FakeSystemUpdate m_systemUpdate;
#else
    Network m_network;
    QProcess m_process;
    UbuntuOne::SSOService m_service;
    SystemUpdate m_systemUpdate;
#endif

    void checkForUpdates();
    QString getClickCommand();
    bool getCheckForCredentials();
    bool enableAutopilotMode();
    void reportCheckState();
    void updateNotAvailable();
    void setCurrentDetailedVersion();
};

}

#endif // UPDATEMANAGER_H
