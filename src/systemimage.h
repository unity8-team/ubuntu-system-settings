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
public:
    explicit QSystemImage(QObject *parent = 0);
    ~QSystemImage();

    Q_PROPERTY(int downloadMode READ downloadMode
               WRITE setDownloadMode NOTIFY downloadModeChanged)
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
    Q_PROPERTY(QString availableVersion READ availableVersion
               NOTIFY availableVersionChanged)
    Q_PROPERTY(int updateSize READ updateSize
               NOTIFY updateSizeChanged)
    Q_PROPERTY(QString errorReason READ errorReason
               NOTIFY errorReasonChanged)

    int downloadMode();
    void setDownloadMode(const int &downloadMode);

    QString deviceName() const;
    QString channelName() const;
    QString currentUbuntuBuildNumber() const;
    QString currentDeviceBuildNumber() const;
    QString currentCustomBuildNumber() const;
    QVariantMap detailedVersionDetails() const;
    int currentBuildNumber() const;
    int targetBuildNumber() const;
    QDateTime lastUpdateDate() const;
    QDateTime lastCheckDate() const;

    bool updateAvailable();
    bool downloading();
    QString availableVersion();
    int updateSize();
    QString errorReason();

    Q_INVOKABLE void checkForUpdate();
    Q_INVOKABLE void downloadUpdate();
    Q_INVOKABLE void forceAllowGSMDownload();
    Q_INVOKABLE void applyUpdate();
    Q_INVOKABLE void cancelUpdate();
    Q_INVOKABLE void pauseDownload();
    Q_INVOKABLE void productionReset();
    Q_INVOKABLE void factoryReset();
    Q_INVOKABLE bool checkTarget() const;

signals:
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
    void availableVersionChanged();
    void updateSizeChanged();
    void errorReasonChanged();

    void downloadModeChanged();
    void updateNotFound();
    void updateProcessFailed(const QString &reason);

    void rebooting(const bool status);
    void updateFailed(const int &consecutiveFailureCount, const QString &lastReason);
    void updateDownloaded();
    void downloadStarted();
    void updatePaused(const int &percentage);
    void updateAvailableStatus(const bool isAvailable,
                               const bool downloading,
                               const QString availableVersion,
                               const int updateSize,
                               const QString lastUpdateDate,
                               const QString errorReason);

    void updateProgress(const int &percentage, const double &eta);

private slots:
    void slotNameOwnerChanged(const QString&, const QString&, const QString&);
    void settingsChanged(const QString &key, const QString &newvalue);
    void availableStatusChanged(const bool isAvailable,
                                const bool downloading,
                                const QString &availableVersion,
                                const int &updateSize,
                                const QString &lastUpdateDate,
                                const QString &errorReason);

private:
    int m_currentBuildNumber;
    QMap<QString, QVariant> m_detailedVersion;
    QDateTime m_lastUpdateDate;
    int m_downloadMode;
    QDBusConnection m_systemBusConnection;
    QDBusServiceWatcher m_serviceWatcher;
    QDBusInterface m_systemServiceIface;

    QDateTime m_lastCheckDate;
    QString m_channelName;
    int m_targetBuildNumber;
    QString m_deviceName;

    bool m_updateAvailable;
    bool m_downloading;
    QString m_availableVersion;
    int m_updateSize;
    QString m_errorReason;

    // Synchronously initialize properties from the Information call.
    void initializeProperties();
    // Sets up connections on the DBus interface.
    void setUpInterface();
};

#endif // QSYSTEMIMAGE_H