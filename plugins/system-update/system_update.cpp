/*
 * Copyright (C) 2013-2014 Canonical Ltd
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
 * Didier Roche <didier.roche@canonical.com>
 * Diego Sarmentero <diego.sarmentero@canonical.com>
 *
*/

#include "system_update.h"
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

// FIXME: need to do this better including #include "../../src/i18n.h"
// and linking to it
#include <libintl.h>
QString _(const char *text)
{
    return QString::fromUtf8(dgettext(0, text));
}

namespace UpdatePlugin {

SystemUpdate::SystemUpdate(QObject *parent) :
    QObject(parent),
    m_currentBuildNumber(-1),
    m_downloadMode(-1),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_SystemServiceIface ("com.canonical.SystemImage",
                         "/Service",
                         "com.canonical.SystemImage",
                         m_systemBusConnection)
{
    update = nullptr;

    connect(&m_SystemServiceIface, SIGNAL(UpdateAvailableStatus(bool, bool, QString, int, QString, QString)),
               this, SLOT(ProcessAvailableStatus(bool, bool, QString, int, QString, QString)));
    // signals to forward directly to QML
    connect(&m_SystemServiceIface, SIGNAL(UpdateProgress(int, double)),
                this, SIGNAL(updateProgress(int, double)));
    connect(&m_SystemServiceIface, SIGNAL(UpdateProgress(int, double)),
                this, SLOT(updateDownloadProgress(int, double)));
    connect(&m_SystemServiceIface, SIGNAL(UpdatePaused(int)),
                this, SIGNAL(updatePaused(int)));
    connect(&m_SystemServiceIface, SIGNAL(UpdateDownloaded()),
                this, SIGNAL(updateDownloaded()));
    connect(&m_SystemServiceIface, SIGNAL(UpdateFailed(int, QString)),
                this, SIGNAL(updateFailed(int, QString)));
    connect(&m_SystemServiceIface, SIGNAL(SettingChanged(QString, QString)),
                this, SLOT(ProcessSettingChanged(QString, QString)));
}

SystemUpdate::~SystemUpdate() {
}

void SystemUpdate::checkForUpdate() {
    m_SystemServiceIface.asyncCall("CheckForUpdate");
}

void SystemUpdate::downloadUpdate() {
    m_SystemServiceIface.asyncCall("DownloadUpdate");
}

void SystemUpdate::applyUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("ApplyUpdate");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(reply.value());
}

void SystemUpdate::cancelUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("CancelUpdate");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(_("Can't cancel current request (can't contact service)"));
}

void SystemUpdate::pauseDownload() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("PauseDownload");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(_("Can't pause current request (can't contact service)"));
}

int SystemUpdate::currentBuildNumber() {
    if (m_currentBuildNumber != -1)
        return m_currentBuildNumber;

    QDBusReply<int> reply = m_SystemServiceIface.call("Info");
    if (reply.isValid())
        m_currentBuildNumber = reply.value();

    return m_currentBuildNumber;
}

int SystemUpdate::downloadMode() {
    if (m_downloadMode != -1)
        return m_downloadMode;

    QDBusReply<QString> reply = m_SystemServiceIface.call("GetSetting", "auto_download");
    int default_mode = 1;
    if (reply.isValid()) {
        bool ok;
        int result;
        result = reply.value().toInt(&ok);
        if (ok)
            m_downloadMode = result;
        else
            m_downloadMode = default_mode;
    }
    else
        m_downloadMode = default_mode;
    return m_downloadMode;
}

void SystemUpdate::setDownloadMode(int value) {
    if (m_downloadMode == value)
        return;

    m_downloadMode = value;
    m_SystemServiceIface.asyncCall("SetSetting", "auto_download", QString::number(value));
}

void SystemUpdate::ProcessSettingChanged(QString key, QString newvalue) {
    if(key == "auto_download") {
        bool ok;
        int newintValue;
        newintValue = newvalue.toInt(&ok);
        if (ok) {
            m_downloadMode = newintValue;
            Q_EMIT downloadModeChanged();
        }
    }
}

void SystemUpdate::ProcessAvailableStatus(bool isAvailable,
                                          bool downloading,
                                          QString availableVersion,
                                          int updateSize,
                                          QString lastUpdateDate,
                                          QString errorReason)
{
    update = new Update(this);
    QString packageName("UbuntuImage");
    update->initializeApplication(packageName, "Ubuntu",
                                  QString::number(this->currentBuildNumber()));

    update->setSystemUpdate(true);
    update->setRemoteVersion(availableVersion);
    update->setBinaryFilesize(updateSize);
    update->setError(errorReason);
    update->setUpdateState(downloading);
    update->setSelected(downloading);
    update->setUpdateAvailable(isAvailable);
    update->setLastUpdateDate(lastUpdateDate);
    update->setIconUrl(QString("file:///usr/share/ubuntu/settings/system/icons/distributor-logo.png"));

    if (update->updateRequired()) {
        Q_EMIT updateAvailable(packageName, update);
    } else {
        Q_EMIT updateNotFound();
    }
}

void SystemUpdate::updateDownloadProgress(int percentage, double eta)
{
    Q_UNUSED(eta);
    if (update != nullptr) {
        update->setDownloadProgress(percentage);
    }
}

}
