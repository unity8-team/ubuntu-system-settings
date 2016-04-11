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

QSystemImage::QSystemImage(QObject *parent) :
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
             SIGNAL(serviceOwnerChanged(QString, QString, QString)),
             this,
             SLOT(slotNameOwnerChanged(QString, QString, QString)));

    setUpInterface();
}

QSystemImage::~QSystemImage() {
}

void QSystemImage::slotNameOwnerChanged(const QString &name,
                                        const QString &oldOwner,
                                        const QString &newOwner) {
    Q_UNUSED (oldOwner);
    Q_UNUSED (newOwner);

    if (name != "com.canonical.SystemImage")
        return;

    if (m_systemServiceIface.isValid())
        setUpInterface();
}

void QSystemImage::setUpInterface() {
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

void QSystemImage::factoryReset() {
    m_systemServiceIface.asyncCall("FactoryReset");
}

void QSystemImage::productionReset() {
    m_systemServiceIface.asyncCall("ProductionReset");
}

void QSystemImage::checkForUpdate() {
    m_systemServiceIface.asyncCall("CheckForUpdate");
}

void QSystemImage::downloadUpdate() {
    m_systemServiceIface.asyncCall("DownloadUpdate");
}

void QSystemImage::forceAllowGSMDownload() {
    m_systemServiceIface.asyncCall("ForceAllowGSMDownload");
}

void QSystemImage::applyUpdate() {
    QDBusReply<QString> reply = m_systemServiceIface.call("ApplyUpdate");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(reply.value());
}

void QSystemImage::cancelUpdate() {
    QDBusReply<QString> reply = m_systemServiceIface.call("CancelUpdate");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(_("Can't cancel current request (can't contact service)"));
}

void QSystemImage::pauseDownload() {
    QDBusReply<QString> reply = m_systemServiceIface.call("PauseDownload");
    if (!reply.isValid())
        Q_EMIT updateProcessFailed(_("Can't pause current request (can't contact service)"));
}

void QSystemImage::initializeProperties() {
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

bool QSystemImage::getIsTargetNewer() const {
    return m_targetBuildNumber > m_currentBuildNumber;
}

QString QSystemImage::deviceName() const {
    return m_deviceName;
}

QString QSystemImage::channelName() const {
    return m_channelName;
}

QDateTime QSystemImage::lastUpdateDate() const {
    return m_lastUpdateDate;
}

QDateTime QSystemImage::lastCheckDate() const {
    return m_lastCheckDate;
}

int QSystemImage::currentBuildNumber() const {
    return m_currentBuildNumber;
}

QString QSystemImage::currentUbuntuBuildNumber() const {
    QString val = m_detailedVersion.value("ubuntu").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

QString QSystemImage::currentDeviceBuildNumber() const {
    QString val = m_detailedVersion.value("device").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

QString QSystemImage::currentCustomBuildNumber() const {
    QString val = m_detailedVersion.value("custom").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

int QSystemImage::targetBuildNumber() const {
    return m_targetBuildNumber;
}

QVariantMap QSystemImage::detailedVersionDetails() const {
    return m_detailedVersion;
}

int QSystemImage::downloadMode() {
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

void QSystemImage::setDownloadMode(const int &downloadMode) {
    if (m_downloadMode == downloadMode)
        return;

    m_downloadMode = downloadMode;
    m_systemServiceIface.asyncCall("SetSetting", "auto_download", QString::number(downloadMode));
}

void QSystemImage::settingsChanged(const QString &key, const QString &newvalue) {
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

// void QSystemImage::ProcessAvailableStatus(bool isAvailable,
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

//     // update->QSystemImage(true);
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
