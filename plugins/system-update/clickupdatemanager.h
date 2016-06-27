/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#ifndef CLICK_UPDATE_MANAGER_H
#define CLICK_UPDATE_MANAGER_H

#include "systemupdate.h"
#include "clicktokendownloader.h"
#include "updatedb.h"

#include <ssoservice.h>
#include <token.h>

#include <QDateTime>
#include <QFile>
#include <QHash>
#include <QJsonDocument>
#include <QList>
#include <QProcess>

// Having the full namespaced name in a slot seems to confuse
// SignalSpy so we need this declaration.
using UbuntuOne::Token;

namespace UpdatePlugin
{
class ClickUpdateManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool authenticated READ authenticated
               NOTIFY authenticatedChanged)
public:
    explicit ClickUpdateManager(QObject *parent = 0);
    // For testing.
    explicit ClickUpdateManager(const QString &dbpath, QObject *parent = 0);
    ~ClickUpdateManager();

    Q_INVOKABLE void check();
    Q_INVOKABLE void check(const QString &packageName);
    Q_INVOKABLE void cancel();
    // Q_INVOKABLE void markInstalled(const QString &packageName, const int &revision);
    // Q_INVOKABLE void setUpdateState(const QString &packageName, const int &revision,
    //                                 const int &state);
    // Q_INVOKABLE void setProgress(const QString &packageName, const int &revision,
    //                              const int &progress);

    Q_INVOKABLE bool isCheckRequired();

    bool authenticated();

private slots:
    void handleMetadata(QNetworkReply *reply);
    void processInstalledClicks(const int &exitCode);
    void handleProcessError(const QProcess::ProcessError &error);

    void handleTokenDownload(Update *update);
    void handleTokenDownloadFailure(Update *update);

    void handleCredentialsFound(const Token &token);
    void handleCredentialsFailed();
    void handleCommunicationErrors();
    void handleCheckStart() { m_checking = true; }
    void handleCheckStop() { m_checking = false; }
    void handleCheckCompleted();

signals:
    void authenticatedChanged();

    void checkStarted();
    void checkCompleted();
    void checkCanceled();
    void checkFailed();

    void networkError();
    void serverError();
    void credentialError();

private:
    void setAuthenticated(const bool authenticated);

    void init();
    // Set up connections on a process instance.
    void initializeProcess();
    void initializeSSOService();
    void initializeApiClient();
    void initializeTokenDownloader(const ClickTokenDownloader *dler);
    void initializeUpdate(const Update *update);

    // Start process of adding remote metadata to each installed click
    void requestClickMetadata();

    // Parses click metadata.
    // Note: This also asks a ClickUpdate to request a click token.
    // TODO: Make this more obvious.
    void parseClickMetadata(const QJsonArray &array);

    // Assert completion of check, signalling if check complete.
    void completionCheck();

    UpdateDb *m_db;
    // Represents the process that we use to query installed clicks.
    QProcess m_process;
    ClickApiClient m_apiClient;
    QHash<QString, Update*> m_updates;

    UbuntuOne::Token m_authToken;
    UbuntuOne::SSOService m_ssoService;
    bool m_authenticated;
    bool m_checking;
};
} // UpdatePlugin

#endif // CLICK_UPDATE_MANAGER_H
