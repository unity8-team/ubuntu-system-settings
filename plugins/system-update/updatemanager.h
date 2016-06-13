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

#ifndef PLUGINS_SYSTEM_UPDATE_MANAGER_H_
#define PLUGINS_SYSTEM_UPDATE_MANAGER_H_

#include <QDebug>

#include <token.h>
#include <ssoservice.h>

#include "clickupdatemetadata.h"
#include "clickupdatechecker.h"
#include "updatemodel.h"
#include "updatestore.h"
#include "systemimage.h"

// Having the full namespaced name in a slot seems to confuse
// SignalSpy so we need this declaration.
using UbuntuOne::Token;

namespace UpdatePlugin
{

class UpdateManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool authenticated READ authenticated
               NOTIFY authenticatedChanged)
    Q_ENUMS(UpdateKind)
    Q_ENUMS(UpdateState)
    Q_ENUMS(Status)
public:
    enum class UpdateKind
    {
        KindApp,
        KindSystem,
        KindUnknown
    };

    enum class UpdateState
    {
        StateUnknown,
        StateAvailable,
        StateUnavailable,
        StateQueuedForDownload,
        StateDownloading,
        StateDownloadingAutomatically,
        StateDownloadPaused,
        StateAutomaticDownloadPaused,
        StateInstalling,
        StateInstallingAutomatically,
        StateInstallPaused,
        StateInstalled,
        StateDownloaded,
        StateFailed
    };

    enum class Status
    {
        StatusIdle,
        StatusCheckingClickUpdates,
        StatusCheckingSystemUpdates,
        StatusCheckingAllUpdates,
        StatusBatchMode, // Installing all updates
        StatusNetworkError,
        StatusServerError
    };

    static UpdateManager *instance();

    bool authenticated();
    UpdateStore *updateStore();

    Q_INVOKABLE void checkForClickUpdates();
    Q_INVOKABLE void cancelCheckForClickUpdates();
    Q_INVOKABLE void retryClickPackage(const QString &packageName, const int &revision);
    Q_INVOKABLE void clickUpdateInstalled(const QString &packageName, const int &revision);

protected:
    explicit UpdateManager(QObject *parent = 0);
    ~UpdateManager();

private slots:
    void onClickUpdateAvailable(const ClickUpdateMetadata *meta);
    void handleCredentialsFound(const Token &token);
    void handleCredentialsFailed();

signals:
    void authenticatedChanged();
    void networkError();
    void serverError();
    void clickUpdateCheckStarted();
    void clickUpdateCheckCompleted();

private:
    static UpdateManager *m_instance;

    void setAuthenticated(const bool authenticated);

    void initializeClickUpdateChecker();
    void initializeSSOService();
    void initializeUpdateStore();

    ClickUpdateChecker m_clickUpChecker;
    UbuntuOne::Token m_token;
    UbuntuOne::SSOService m_ssoService;
    UpdateStore m_updatestore;
    bool m_authenticated;
};

}

#endif // PLUGINS_SYSTEM_UPDATE_MANAGER_H_
