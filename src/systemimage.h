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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/

#ifndef QSYSTEMIMAGE_H
#define QSYSTEMIMAGE_H

#include <QtDBus>
#include <QDBusInterface>
#include <QObject>
#include <QDBusServiceWatcher>


class QSystemImage : public QObject
{
    Q_OBJECT
public:
    explicit QSystemImage(QObject *parent = 0);
    ~QSystemImage();

    Q_PROPERTY(QString downloadMode READ downloadMode
               WRITE setDownloadMode NOTIFY downloadModeChanged)
    Q_PROPERTY(QString channelName READ channelName NOTIFY channelNameChanged)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString currentBuildNumber READ currentBuildNumber
               NOTIFY currentBuildNumberChanged)
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

    // TODO: make this const
    int downloadMode();
    void setDownloadMode(const int &downloadMode);

    const QString deviceName();
    const QString channelName();
    const QString currentUbuntuBuildNumber();
    const QString currentDeviceBuildNumber();
    const QString currentCustomBuildNumber();
    const QVariantMap detailedVersionDetails();
    const int currentBuildNumber();
    const QDateTime lastUpdateDate();
    const QDateTime lastCheckDate();

    Q_INVOKABLE void checkForUpdate();
    Q_INVOKABLE void downloadUpdate();
    Q_INVOKABLE void forceAllowGSMDownload();
    Q_INVOKABLE void applyUpdate();
    Q_INVOKABLE void cancelUpdate();
    Q_INVOKABLE void pauseDownload();
    Q_INVOKABLE void checkTarget();
    Q_INVOKABLE void productionReset();
    Q_INVOKABLE void factoryReset();

signals:
    void currentBuildNumberChanged();
    void deviceNameChanged();
    void channelNameChanged();
    void currentUbuntuBuildNumberChanged();
    void currentDeviceBuildNumberChanged();
    void currentCustomBuildNumberChanged();
    void detailedVersionDetailsChanged();
    void lastUpdateDateChanged();
    void lastCheckDateChanged();

    void downloadModeChanged();
    void updateNotFound();
    void updateProcessFailed(const QString &reason);

    void rebooting(const bool status);
    void updateFailed(const int &consecutiveFailureCount, const QString &lastReason);
    void updateDownloaded();
    void downloadStarted();
    void updatePaused(const int &percentage);
    void updateProgress(const int &percentage, const double &eta);
    void updateAvailableStatus(const bool isAvailable,
                               const bool downloading,
                               const QString &availableVersion,
                               const int &updateSize,
                               const QString &lastUpdateDate,
                               const QString &errorReason);
    void updateProgress(const int &percentage, const double &eta);

private slots:
    void slotNameOwnerChanged(const QString&, const QString&, const QString&);

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
    QString m_objectPath;
    QString m_deviceName;

    void initializeProperties();
};

}

#endif // QSYSTEMIMAGE_H
