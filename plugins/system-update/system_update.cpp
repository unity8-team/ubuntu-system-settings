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
 * Sergio Schvezov <sergio.schvezov@canonical.com>
 *
*/

#include <QEvent>
#include <QDateTime>
#include <QDebug>
#include <QDBusReply>

#include <unistd.h>

// FIXME: need to do this better including #include "../../src/i18n.h"
// and linking to it
#include <libintl.h>

#include "system_update.h"

namespace {
    const QString SYSTEM_IMG_SERVICE = "com.canonical.SystemImage";
    const QString SYSTEM_IMG_PATH = "/Service";
    const QString SYSTEM_IMG_INTERFACE = "com.canonical.SystemImage";
    const QString UBUNTU_APP_NAME = "Ubuntu";
    const QString UBUNTU_LOGO_PATH = "file:///usr/share/ubuntu/settings/system/icons/distributor-logo.png";
    const QString AUTO_DOWNLOAD_KEY = "auto_download";
    const QString DEVICE_KEY = "device";
    const QString UNAVAILABLE_VALUE = "Unavailable";
}

QString _(const char *text)
{
    return QString::fromUtf8(dgettext(0, text));
}

namespace UpdatePlugin {

SystemUpdate::SystemUpdate(QObject *parent) :
    QObject(parent),
    m_currentBuildNumber(-1),
    m_detailedVersion(),
    m_lastUpdateDate(),
    m_downloadMode(-1),
    m_SystemServiceIface (SYSTEM_IMG_SERVICE,
                          SYSTEM_IMG_PATH,
                          SYSTEM_IMG_INTERFACE, 
                          QDBusConnection::systemBus())
{
    update = nullptr;

    qDBusRegisterMetaType<QMap<QString, QString> >();

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
    connect(&m_SystemServiceIface, SIGNAL(Rebooting(bool)),
                this, SIGNAL(rebooting(bool)));
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
    qDebug() <<  __PRETTY_FUNCTION__;

    QDBusReply<QString> reply = m_SystemServiceIface.call("ApplyUpdate");
    if (!reply.isValid()) {
	auto error = reply.error();
        qDebug() << error.name() << ":" << error.message();

        Q_EMIT updateProcessFailed(reply.value());
    }
}

void SystemUpdate::cancelUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("CancelUpdate");
    if (!reply.isValid()) {
	auto error = reply.error();
        qDebug() << error.name() << ":" << error.message();

        Q_EMIT updateProcessFailed(_("Can't cancel current request (can't contact service)"));
    }
}

void SystemUpdate::pauseDownload() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("PauseDownload");
    if (!reply.isValid()) {
	auto error = reply.error();
        qDebug() << error.name() << ":" << error.message();

        Q_EMIT updateProcessFailed(_("Can't pause current request (can't contact service)"));
    }
}

void SystemUpdate::setCurrentDetailedVersion() {
    QDBusPendingReply<int, QString, QString, QString, QMap<QString, QString> > reply = m_SystemServiceIface.call("Info");
    reply.waitForFinished();
    if (reply.isValid()) {
        m_currentBuildNumber = reply.argumentAt<0>();
        m_lastUpdateDate = QDateTime::fromString(reply.argumentAt<3>(), Qt::ISODate);
        m_detailedVersion = reply.argumentAt<4>();
        Q_EMIT versionChanged();
    } else {
        qWarning() << "Error when retrieving version information: " << reply.error();
    }
}

bool SystemUpdate::checkTarget() {
    int target = 0;
    int current = 0;
    QDBusPendingReply<QMap<QString, QString> > reply = m_SystemServiceIface.call("Information");
    reply.waitForFinished();
    if (reply.isValid()) {
        QMap<QString, QString> result = reply.argumentAt<0>();
        target = result.value("target_build_number", "0").toInt();
        current = result.value("current_build_number", "0").toInt();
    } else {
        qWarning() << "Error when retrieving version information: " << reply.error();
    }

    return target > current;
}


QDateTime SystemUpdate::lastUpdateDate() {
    if (!m_lastUpdateDate.isValid())
        setCurrentDetailedVersion();

    return m_lastUpdateDate;
}

int SystemUpdate::currentBuildNumber() {
    if (m_currentBuildNumber == -1)
        setCurrentDetailedVersion();

    return m_currentBuildNumber;
}

QString SystemUpdate::currentUbuntuBuildNumber() {
    if (!m_detailedVersion.contains("ubuntu"))
        setCurrentDetailedVersion();

    return m_detailedVersion.value("ubuntu", UNAVAILABLE_VALUE);
}

QString SystemUpdate::currentDeviceBuildNumber() {
    if (!m_detailedVersion.contains(DEVICE_KEY))
        setCurrentDetailedVersion();

    return m_detailedVersion.value(DEVICE_KEY, UNAVAILABLE_VALUE);
}

int SystemUpdate::downloadMode() {
    if (m_downloadMode != -1)
        return m_downloadMode;

    QDBusReply<QString> reply = m_SystemServiceIface.call("GetSetting", AUTO_DOWNLOAD_KEY);
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
    m_SystemServiceIface.asyncCall("SetSetting", AUTO_DOWNLOAD_KEY, QString::number(value));
}

void SystemUpdate::ProcessSettingChanged(QString key, QString newvalue) {
    if(key == AUTO_DOWNLOAD_KEY) {
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
    qDebug() <<  __PRETTY_FUNCTION__;

    update = new Update(this);
    QString packageName(UBUNTU_PACKAGE_NAME);
    update->initializeApplication(packageName, UBUNTU_APP_NAME,
                                  QString::number(this->currentBuildNumber()));

    update->setSystemUpdate(true);
    update->setRemoteVersion(availableVersion);
    update->setBinaryFilesize(updateSize);
    update->setError(errorReason);
    update->setUpdateState(downloading);
    update->setSelected(downloading);
    update->setUpdateAvailable(isAvailable);
    update->setLastUpdateDate(lastUpdateDate);
    update->setIconUrl(UBUNTU_LOGO_PATH);

    if (update->updateRequired()) {
	qDebug() << "Update available";
        Q_EMIT updateAvailable(packageName, update);
    } else {
	qDebug() << "Update not found";
        Q_EMIT updateNotFound();
    }

    if (downloading) {
        update->setSelected(true);
    }
}

void SystemUpdate::updateDownloadProgress(int percentage, double eta)
{
    qDebug() <<  __PRETTY_FUNCTION__ << percentage << eta;
    Q_UNUSED(eta);
    if (update != nullptr) {
        update->setDownloadProgress(percentage);
    }
}

}
