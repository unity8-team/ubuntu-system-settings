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

#ifndef QSYSTEMIMAGE_H
#define QSYSTEMIMAGE_H

#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QObject>
#include <QDebug>
#include <QtDBus>

class QSystemImage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int downloadMode READ downloadMode
               WRITE setDownloadMode NOTIFY downloadModeChanged)
    Q_PROPERTY(int failuresBeforeWarning READ failuresBeforeWarning
               NOTIFY failuresBeforeWarningChanged)
    Q_PROPERTY(bool checkingForUpdates READ checkingForUpdates
               NOTIFY checkingForUpdatesChanged)
    Q_PROPERTY(QString channelName READ channelName NOTIFY channelNameChanged)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(int currentBuildNumber READ currentBuildNumber
               NOTIFY currentBuildNumberChanged)
    Q_PROPERTY(int targetBuildNumber READ targetBuildNumber
               NOTIFY targetBuildNumberChanged)
    Q_PROPERTY(QString currentUbuntuBuildNumber READ currentUbuntuBuildNumber
               NOTIFY currentUbuntuBuildNumberChanged)
    Q_PROPERTY(QString currentDeviceBuildNumber READ currentDeviceBuildNumber
               NOTIFY currentDeviceBuildNumberChanged)
    Q_PROPERTY(QString currentCustomBuildNumber READ currentCustomBuildNumber
               NOTIFY currentCustomBuildNumberChanged)
    Q_PROPERTY(QVariantMap detailedVersionDetails READ detailedVersionDetails
               NOTIFY detailedVersionDetailsChanged)
    Q_PROPERTY(QDateTime lastUpdateDate READ lastUpdateDate
               NOTIFY lastUpdateDateChanged)
    Q_PROPERTY(QDateTime lastCheckDate READ lastCheckDate
               NOTIFY lastCheckDateChanged)
    Q_PROPERTY(bool updateAvailable READ updateAvailable
               NOTIFY updateAvailableChanged)
    Q_PROPERTY(bool downloading READ downloading
               NOTIFY downloadingChanged)
    Q_PROPERTY(int updateSize READ updateSize
               NOTIFY updateSizeChanged)
    Q_PROPERTY(QString errorReason READ errorReason
               NOTIFY errorReasonChanged)
    Q_PROPERTY(QString versionTag READ versionTag
               NOTIFY versionTagChanged)
public:
    explicit QSystemImage(QObject *parent = nullptr);
    explicit QSystemImage(const QDBusConnection &dbus, QObject *parent = nullptr);
    ~QSystemImage();

    bool checkingForUpdates() const;
    int downloadMode();
    void setDownloadMode(const int &downloadMode);
    int failuresBeforeWarning();

    QString deviceName() const;
    QString channelName() const;
    QString currentUbuntuBuildNumber() const;
    QString currentDeviceBuildNumber() const;
    QString currentCustomBuildNumber() const;
    QVariantMap detailedVersionDetails() const;
    int currentBuildNumber() const;
    int targetBuildNumber() const;

    bool updateAvailable();
    bool downloading();
    int updateSize();
    QString errorReason();
    QString versionTag();
    QDateTime lastUpdateDate() const;
    QDateTime lastCheckDate() const;

    Q_INVOKABLE void checkForUpdate();
    Q_INVOKABLE void downloadUpdate();
    Q_INVOKABLE void forceAllowGSMDownload();
    Q_INVOKABLE void applyUpdate();
    Q_INVOKABLE QString cancelUpdate();
    Q_INVOKABLE QString pauseDownload();
    Q_INVOKABLE void productionReset();
    Q_INVOKABLE void factoryReset();
    Q_INVOKABLE bool checkTarget() const;

Q_SIGNALS:
    void checkingForUpdatesChanged();
    void currentBuildNumberChanged();
    void deviceNameChanged();
    void channelNameChanged();
    void currentUbuntuBuildNumberChanged();
    void currentDeviceBuildNumberChanged();
    void currentCustomBuildNumberChanged();
    void targetBuildNumberChanged();
    void detailedVersionDetailsChanged();
    void lastUpdateDateChanged();
    void lastCheckDateChanged();
    void updateAvailableChanged();
    void downloadingChanged();
    void failuresBeforeWarningChanged();
    void updateSizeChanged();
    void errorReasonChanged();
    void versionTagChanged();
    void downloadModeChanged();
    void updateProcessFailed(const QString &reason);
    void updateProcessing();
    void rebooting(const bool status);
    void updateFailed(const int &consecutiveFailureCount,
                      const QString &lastReason);
    void updateDownloaded();
    void downloadStarted();
    void updatePaused(const int &percentage);
    void updateAvailableStatus(const bool isAvailable,
                               const bool downloading,
                               const QString &availableVersion,
                               const int &updateSize,
                               const QString &lastUpdateDate,
                               const QString &errorReason);

    void updateProgress(const int &percentage, const double &eta);

protected Q_SLOTS:
    void slotNameOwnerChanged(const QString&, const QString&, const QString&);
    void settingsChanged(const QString &key, const QString &newvalue);
    void availableStatusChanged(const bool isAvailable,
                                const bool downloading,
                                const QString &availableVersion,
                                const int &updateSize,
                                const QString &lastUpdateDate,
                                const QString &errorReason);

protected:
    void setCheckingForUpdates(const bool checking);
    void setDeviceName(const QString &deviceName);
    void setChannelName(const QString &channelName);
    void setDetailedVersionDetails(const QVariantMap &detailedVersionDetails);
    void setCurrentBuildNumber(const int &currentBuildNumber);
    void setTargetBuildNumber(const int &targetBuildNumber);
    void setLastUpdateDate(const QDateTime &lastUpdateDate);
    void setLastCheckDate(const QDateTime &lastCheckDate);
    void setUpdateAvailable(const bool updateAvailable);
    void setDownloading(const bool downloading);
    void setUpdateSize(const int &updateSize);
    void setErrorReason(const QString &errorReason);

private:
    // Synchronously initialize properties from the Information call.
    void initializeProperties();
    // Sets up connections on the DBus interface.
    void setUpInterface();
    int getSetting(const QString &setting, const int &defaultValue);

    bool m_checkingForUpdates = false;
    int m_currentBuildNumber = 0;
    QMap<QString, QVariant> m_detailedVersion;
    QDateTime m_lastUpdateDate = QDateTime();
    int m_downloadMode = -1;
    int m_failuresBeforeWarning = -1;

    // QDBusConnection m_dbus;
    QDBusServiceWatcher m_watcher;
    QDBusInterface m_iface;

    QDateTime m_lastCheckDate = QDateTime();
    QString m_channelName = QString::null;
    int m_targetBuildNumber = -1;
    QString m_deviceName = QString::null;

    bool m_updateAvailable = false;
    bool m_downloading = false;
    int m_updateSize = 0;
    QString m_errorReason = QString::null;
};

#endif // QSYSTEMIMAGE_H
