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
 * Authors:
 * Didier Roche <didier.roche@canonical.com>
 * Diego Sarmentero <diego.sarmentero@canonical.com>
 * Sergio Schvezov <sergio.schvezov@canonical.com>
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/

#include "systemimage.h"
#include <QEvent>
#include <QDateTime>
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
    m_downloadMode(-1),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_serviceWatcher ("com.canonical.SystemImage",
                      m_systemBusConnection,
                      QDBusServiceWatcher::WatchForOwnerChange),
    m_systemServiceIface ("com.canonical.SystemImage",
                         "/Service",
                         "com.canonical.SystemImage",
                         m_systemBusConnection)
{

    qDBusRegisterMetaType<QMap<QString, QString> >();


    connect (&m_serviceWatcher,
             SIGNAL(serviceOwnerChanged (QString, QString, QString)),
             this,
             SLOT(slotNameOwnerChanged (QString, QString, QString)));

    setUpInterface();
}

SystemUpdate::~SystemUpdate() {
}

void SystemUpdate::slotNameOwnerChanged(QString name,
                                        QString oldOwner,
                                        QString newOwner) {
    Q_UNUSED (oldOwner);
    Q_UNUSED (newOwner);

    if (name != "com.canonical.SystemImage")
        return;

    if (m_systemServiceIface.isValid())
        setUpInterface();
}

void SystemUpdate::setUpInterface() {
    connect(&m_systemServiceIface,
            SIGNAL(UpdateAvailableStatus(bool, bool, QString, int, QString,
                                         QString)),
            this,
            SIGNAL(updateAvailableStatus(bool, bool, QString, int, QString,
                                         QString)));
    connect(&m_systemServiceIface, SIGNAL(UpdateProgress(int, double)),
                this, SIGNAL(updateProgress(int, double)));
    connect(&m_systemServiceIface, SIGNAL(UpdateProgress(int, double)),
                this, SIGNAL(updateProgress(int, double)));
    connect(&m_systemServiceIface, SIGNAL(UpdatePaused(int)),
                this, SIGNAL(updatePaused(int)));
    connect(&m_systemServiceIface, SIGNAL(DownloadStarted()),
                this, SIGNAL(downloadStarted()));
    connect(&m_systemServiceIface, SIGNAL(UpdateDownloaded()),
                this, SIGNAL(updateDownloaded()));
    connect(&m_systemServiceIface, SIGNAL(UpdateFailed(int, QString)),
                this, SIGNAL(updateFailed(int, QString)));
    connect(&m_systemServiceIface, SIGNAL(Rebooting(bool)),
                this, SIGNAL(rebooting(bool)));

    connect(&m_systemServiceIface, SIGNAL(SettingChanged(QString, QString)),
                this, SLOT(settingsChanged(QString, QString)));

    initializeProperties();
}

void SystemUpdate::factoryReset() {
    m_systemServiceIface.asyncCall("FactoryReset");
}

void SystemUpdate::factoryReset() {
    m_systemServiceIface.asyncCall("FactoryReset");
}

void SystemUpdate::productionReset() {
    m_systemServiceIface.asyncCall("ProductionReset");
}

void SystemUpdate::checkForUpdate() {
    m_systemServiceIface.asyncCall("CheckForUpdate");
}

void SystemUpdate::downloadUpdate() {
    m_systemServiceIface.asyncCall("DownloadUpdate");
}

void SystemUpdate::forceAllowGSMDownload() {
    m_systemServiceIface.asyncCall("ForceAllowGSMDownload");
}

void SystemUpdate::applyUpdate() {
    QDBusReply<QString> reply = m_systemServiceIface.call("ApplyUpdate");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(reply.value());
}

void SystemUpdate::cancelUpdate() {
    QDBusReply<QString> reply = m_systemServiceIface.call("CancelUpdate");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(_("Can't cancel current request (can't contact service)"));
}

void SystemUpdate::pauseDownload() {
    QDBusReply<QString> reply = m_systemServiceIface.call("PauseDownload");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(_("Can't pause current request (can't contact service)"));
}

void SystemUpdate::initializeProperties() {
    QDBusPendingReply<QMap<QString, QString> > reply = m_systemServiceIface.call("Information");
    reply.waitForFinished();
    if (reply.isValid()) {
        QMap<QString, QString> result = reply.argumentAt<0>();

        m_currentBuildNumber = result["current_build_number"].toInt();
        Q_EMIT currentBuildNumberChanged();

        m_targetBuildNumber = result["target_build_number"].toInt();
        Q_EMIT targetBuildNumberChanged();

        m_channelName = result["channel_name"];
        Q_EMIT channelNameChanged();

        m_deviceName = result["device_name"];
        Q_EMIT deviceNameChanged();

        m_lastUpdateDate = QDateTime::fromString(result["last_update_date"], Qt::ISODate);
        Q_EMIT lastUpdateDateChanged();

        m_lastCheckDate = QDateTime::fromString(result["last_check_date"], Qt::ISODate);
        Q_EMIT lastCheckDateChanged();

        QMap<QString, QVariant> details;
        QStringList keyvalue = result["version_detail"].split(",", QString::SkipEmptyParts);
        for (int i = 0; i < keyvalue.size(); ++i) {
            QStringList pair = keyvalue.at(i).split("=");
            details[pair[0]] = QVariant(pair[1]);
        }
        m_detailedVersion = details;
        Q_EMIT detailedVersionDetailsChanged();
    } else {
        qWarning() << "Error when retrieving version information: " << reply.error();
    }
}

const bool SystemUpdate::checkTarget() {
    return m_targetBuildNumber > m_currentBuildNumber;
}

const QString SystemUpdate::deviceName() {
    return m_deviceName;
}

const QString SystemUpdate::channelName() {
    return m_channelName;
}

const QDateTime SystemUpdate::lastUpdateDate() {
    return m_lastUpdateDate;
}

const QDateTime SystemUpdate::lastCheckDate() {
    return m_lastCheckDate;
}

const int SystemUpdate::currentBuildNumber() {
    return m_currentBuildNumber;
}

const QString SystemUpdate::currentUbuntuBuildNumber() {
    QString val = m_detailedVersion.value("ubuntu").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

const QString SystemUpdate::currentDeviceBuildNumber() {
    QString val = m_detailedVersion.value("device").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

const QString SystemUpdate::currentCustomBuildNumber() {
    QString val = m_detailedVersion.value("custom").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

const QVariantMap SystemUpdate::detailedVersionDetails() {
    return m_detailedVersion;
}

int SystemUpdate::downloadMode() {
    if (m_downloadMode != -1)
        return m_downloadMode;

    // TODO: read settings at object creation instead of blocking here?
    QDBusReply<QString> reply = m_systemServiceIface.call("GetSetting", "auto_download");
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

void SystemUpdate::setDownloadMode(const int &value) {
    if (m_downloadMode == value)
        return;

    m_downloadMode = value;
    m_systemServiceIface.asyncCall("SetSetting", "auto_download", QString::number(value));
}

void SystemUpdate::settingsChanged(const QString &key, const QString &newvalue) {
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

// void SystemUpdate::ProcessAvailableStatus(bool isAvailable,
//                                           bool downloading,
//                                           QString availableVersion,
//                                           int updateSize,
//                                           QString lastUpdateDate,
//                                           QString errorReason)
// {
//     // update = new Update(this);
//     // QString packageName(UBUNTU_PACKAGE_NAME);
//     // update->initializeApplication(packageName, "Ubuntu",
//     //                               QString::number(this->currentBuildNumber()));

//     // update->setSystemUpdate(true);
//     // update->setRemoteVersion(availableVersion);
//     // update->setBinaryFilesize(updateSize);
//     // update->setError(errorReason);
//     // update->setUpdateState(downloading);
//     // update->setSelected(downloading);
//     // update->setUpdateAvailable(isAvailable);
//     // update->setLastUpdateDate(lastUpdateDate);
//     // update->setIconUrl(QString("file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"));

//     // if (update->updateRequired()) {
//     //     Q_EMIT updateAvailable(packageName, update);
//     // } else {
//     //     Q_EMIT updateNotFound();
//     // }

//     // if (downloading) {
//     //     update->setSelected(true);
//     // }
// }

}
