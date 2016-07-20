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

QSystemImage::QSystemImage(QObject *parent)
    : QSystemImage(QDBusConnection::systemBus(), parent)
{
}

QSystemImage::QSystemImage(const QDBusConnection &dbus, QObject *parent)
    : QObject(parent)
    , m_detailedVersion()
    , m_lastUpdateDate()
    , m_dbus(dbus)
    , m_watcher("com.canonical.SystemImage", m_dbus,
                QDBusServiceWatcher::WatchForOwnerChange)
    , m_iface("com.canonical.SystemImage", "/Service",
              "com.canonical.SystemImage", m_dbus)
    , m_lastCheckDate()
{
    qDBusRegisterMetaType<QMap<QString, QString> >();

    connect(&m_watcher, SIGNAL(serviceOwnerChanged(QString, QString, QString)),
            this, SLOT(slotNameOwnerChanged(QString, QString, QString)));

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

    if (m_iface.isValid())
        setUpInterface();
}

void QSystemImage::setUpInterface() {
    connect(&m_iface,
            SIGNAL(UpdateAvailableStatus(bool, bool, QString, int, QString,
                                         QString)),
            this,
            SIGNAL(updateAvailableStatus(bool, bool, QString, int, QString,
                                         QString)));

    connect(&m_iface,
            SIGNAL(UpdateAvailableStatus(bool, bool, QString, int, QString,
                                         QString)),
            this,
            SLOT(availableStatusChanged(bool, bool, QString, int, QString,
                                          QString)));

    connect(&m_iface, SIGNAL(UpdateProgress(int, double)),
                this, SIGNAL(updateProgress(int, double)));
    connect(&m_iface, SIGNAL(UpdateProgress(int, double)),
                this, SIGNAL(updateProgress(int, double)));
    connect(&m_iface, SIGNAL(UpdatePaused(int)),
                this, SIGNAL(updatePaused(int)));
    connect(&m_iface, SIGNAL(DownloadStarted()),
                this, SIGNAL(downloadStarted()));
    connect(&m_iface, SIGNAL(UpdateDownloaded()),
                this, SIGNAL(updateDownloaded()));
    connect(&m_iface, SIGNAL(UpdateFailed(int, QString)),
                this, SIGNAL(updateFailed(int, QString)));
    connect(&m_iface, SIGNAL(Rebooting(bool)),
                this, SIGNAL(rebooting(bool)));

    connect(&m_iface, SIGNAL(SettingChanged(QString, QString)),
                this, SLOT(settingsChanged(QString, QString)));

    initializeProperties();
}

void QSystemImage::factoryReset() {
    m_iface.asyncCall("FactoryReset");
}

void QSystemImage::productionReset() {
    m_iface.asyncCall("ProductionReset");
}

void QSystemImage::checkForUpdate() {
    m_iface.asyncCall("CheckForUpdate");
}

void QSystemImage::downloadUpdate() {
    m_iface.asyncCall("DownloadUpdate");
}

void QSystemImage::forceAllowGSMDownload() {
    m_iface.asyncCall("ForceAllowGSMDownload");
}

void QSystemImage::applyUpdate() {
    QDBusReply<QString> reply = m_iface.call("ApplyUpdate");
    if (reply.isValid()) {
        Q_EMIT updateProcessing();
    } else {
        Q_EMIT updateProcessFailed(reply.value());
    }
}

QString QSystemImage::cancelUpdate() {
    QDBusPendingReply<QString> reply = m_iface.call("CancelUpdate");
    reply.waitForFinished();
    if (reply.isValid()) {
        return reply.argumentAt<0>();
    } else {
        qWarning() << reply.error().message();
        return _("Can't cancel current request (can't contact service)");
    }
}

QString QSystemImage::pauseDownload() {
    QDBusPendingReply<QString> reply = m_iface.call("PauseDownload");
    reply.waitForFinished();
    if (reply.isValid()) {
        return reply.argumentAt<0>();
    } else {
        qWarning() << reply.error().message();
        return _("Can't pause current request (can't contact service)");
    }
}

void QSystemImage::initializeProperties() {
    QDBusPendingReply<QMap<QString, QString> > reply =
        m_iface.call("Information");
    reply.waitForFinished();
    if (reply.isValid()) {
        QMap<QString, QString> result = reply.argumentAt<0>();

        setCurrentBuildNumber(result["current_build_number"].toInt());
        setTargetBuildNumber(result["target_build_number"].toInt());
        setChannelName(result["channel_name"]);
        setDeviceName(result["device_name"]);
        setLastUpdateDate(QDateTime::fromString(result["last_update_date"],
                                                Qt::ISODate));
        setLastCheckDate(QDateTime::fromString(result["last_check_date"],
                                               Qt::ISODate));

        QMap<QString, QVariant> details;
        QStringList keyvalue = result["version_detail"].split(
            ",", QString::SkipEmptyParts
        );
        for (int i = 0; i < keyvalue.size(); ++i) {
            QStringList pair = keyvalue.at(i).split("=");
            details[pair[0]] = QVariant(pair[1]);
        }
        setDetailedVersionDetails(details);
    } else {
        qWarning() << "Error when retrieving version information: "
                   << reply.error();
    }

    QDBusReply<QString> dlModeReply = m_iface.call("GetSetting",
                                                   "auto_download");
    int default_mode = 1;
    if (dlModeReply.isValid()) {
        bool ok;
        int result;
        result = dlModeReply.value().toInt(&ok);
        if (ok) {
            m_downloadMode = result;
        } else {
            m_downloadMode = default_mode;
        }
    } else {
        qWarning() << "Error setting download mode"
                   << dlModeReply.error().message();
        m_downloadMode = default_mode;
    }
}

bool QSystemImage::checkTarget() const
{
    return m_targetBuildNumber > m_currentBuildNumber;
}

QString QSystemImage::deviceName() const
{
    return m_deviceName;
}

QString QSystemImage::channelName() const
{
    return m_channelName;
}

QDateTime QSystemImage::lastUpdateDate() const
{
    return m_lastUpdateDate;
}

QDateTime QSystemImage::lastCheckDate() const
{
    return m_lastCheckDate;
}

bool QSystemImage::updateAvailable()
{
    return m_updateAvailable;
}

bool QSystemImage::downloading()
{
    return m_downloading;
}

int QSystemImage::updateSize()
{
    return m_updateSize;
}

QString QSystemImage::errorReason()
{
    return m_errorReason;
}

QString QSystemImage::versionTag()
{
    QString val = m_detailedVersion.value("tag").toString();
    return val.isEmpty() ? "" : val;
}

int QSystemImage::currentBuildNumber() const
{
    return m_currentBuildNumber;
}

QString QSystemImage::currentUbuntuBuildNumber() const
{
    QString val = m_detailedVersion.value("ubuntu").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

QString QSystemImage::currentDeviceBuildNumber() const
{
    QString val = m_detailedVersion.value("device").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

QString QSystemImage::currentCustomBuildNumber() const
{
    QString val = m_detailedVersion.value("custom").toString();
    return val.isEmpty() ? _("Unavailable") : val;
}

int QSystemImage::targetBuildNumber() const
{
    return m_targetBuildNumber;
}

QVariantMap QSystemImage::detailedVersionDetails() const
{
    return m_detailedVersion;
}

int QSystemImage::downloadMode()
{
    return m_downloadMode;
}

void QSystemImage::setDownloadMode(const int &downloadMode) {
    if (m_downloadMode == downloadMode) {
        return;
    }

    if (downloadMode < 0 || downloadMode > 2) {
        return;
    }

    m_downloadMode = downloadMode;
    m_iface.asyncCall("SetSetting", "auto_download",
                      QString::number(downloadMode));
}

void QSystemImage::settingsChanged(const QString &key,
                                   const QString &newvalue) {
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

void QSystemImage::availableStatusChanged(const bool isAvailable,
                                          const bool downloading,
                                          const QString &availableVersion,
                                          const int &updateSize,
                                          const QString &lastUpdateDate,
                                          const QString &errorReason)
{
    setUpdateAvailable(isAvailable);
    setDownloading(downloading);

    bool ok;
    int targetBuildNumber = availableVersion.toInt(&ok);
    if (ok) {
        setTargetBuildNumber(targetBuildNumber);
    } else {
        qWarning() << "Failed to parse availableVersion" << availableVersion;
    }

    setUpdateSize(updateSize);

    QDateTime lud = QDateTime::fromString(lastUpdateDate, Qt::ISODate);
    setLastUpdateDate(lud);

    setErrorReason(errorReason);
}

void QSystemImage::setDeviceName(const QString &deviceName)
{
    if (m_deviceName != deviceName) {
        m_deviceName = deviceName;
        Q_EMIT deviceNameChanged();
    }
}

void QSystemImage::setChannelName(const QString &channelName)
{
    if (m_channelName != channelName) {
        m_channelName = channelName;
        Q_EMIT channelNameChanged();
    }
}

void QSystemImage::setDetailedVersionDetails(const QVariantMap &detailedVersionDetails)
{
    if (m_detailedVersion != detailedVersionDetails) {
        m_detailedVersion = detailedVersionDetails;
        Q_EMIT detailedVersionDetailsChanged();

        // These properties are read from m_detailedVersion, so notify.
        Q_EMIT currentUbuntuBuildNumberChanged();
        Q_EMIT currentDeviceBuildNumberChanged();
        Q_EMIT currentCustomBuildNumberChanged();

        if(m_detailedVersion.contains("tag")) {
            Q_EMIT versionTagChanged();
        }
    }
}

void QSystemImage::setCurrentBuildNumber(const int &currentBuildNumber)
{
    if (m_currentBuildNumber != currentBuildNumber) {
        m_currentBuildNumber = currentBuildNumber;
        Q_EMIT currentBuildNumberChanged();
    }
}

void QSystemImage::setTargetBuildNumber(const int &targetBuildNumber)
{
    if (m_targetBuildNumber != targetBuildNumber) {
        m_targetBuildNumber = targetBuildNumber;
        Q_EMIT targetBuildNumberChanged();
    }
}

void QSystemImage::setLastUpdateDate(const QDateTime &lastUpdateDate)
{
    if (m_lastUpdateDate != lastUpdateDate) {
        m_lastUpdateDate = lastUpdateDate;
        Q_EMIT lastUpdateDateChanged();
    }
}

void QSystemImage::setLastCheckDate(const QDateTime &lastCheckDate)
{
    if (m_lastCheckDate != lastCheckDate) {
        m_lastCheckDate = lastCheckDate;
        Q_EMIT lastCheckDateChanged();
    }
}

void QSystemImage::setUpdateAvailable(const bool updateAvailable)
{
    if (m_updateAvailable != updateAvailable) {
        m_updateAvailable = updateAvailable;
        Q_EMIT updateAvailableChanged();
    }
}

void QSystemImage::setDownloading(const bool downloading)
{
    if (m_downloading != downloading) {
        m_downloading = downloading;
        Q_EMIT downloadingChanged();
    }
}

void QSystemImage::setUpdateSize(const int &updateSize)
{
    if (m_updateSize != updateSize) {
        m_updateSize = updateSize;
        Q_EMIT updateSizeChanged();
    }
}

void QSystemImage::setErrorReason(const QString &errorReason)
{
    if (m_errorReason != errorReason) {
        m_errorReason = errorReason;
        Q_EMIT errorReasonChanged();
    }
}

