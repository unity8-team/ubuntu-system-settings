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

#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QObject>
#include <QtDBus>

/**
  *
 @brief      Provides bindings for the D-Bus API on the bus name
             com.canonical.SystemImage, object path /Service, and interface
             com.canonical.SystemImage.
*/
class QSystemImage : public QObject
{
    Q_OBJECT
public:
    /*!
     \brief

     \param parent
    */
    explicit QSystemImage(QObject *parent = 0);
    /*!
     \brief

    */
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

    // TODO: make this const
    /*!
     \brief

     \return int
    */
    int downloadMode();
    /*!
     \brief

     \param downloadMode
    */
    void setDownloadMode(const int &downloadMode);

    /*!
     \brief

     \return QString
    */
    QString deviceName() const;
    /*!
     \brief

     \return QString
    */
    QString channelName() const;
    /*!
     \brief Return channel name.

     \return QString
    */
    QString currentUbuntuBuildNumber() const;
    /*!
     \brief Return current Ubuntu build number.

     \return QString
    */
    QString currentDeviceBuildNumber() const;
    /*!
     \brief Return custom build number.

     \return QString
    */
    QString currentCustomBuildNumber() const;
    /*!
     \brief Return map of detailed version details.

     \return QVariantMap
    */
    QVariantMap detailedVersionDetails() const;
    /*!
     \brief Return current build number.

     \return int
    */
    int currentBuildNumber() const;
    /*!
     \brief Return target build number.

     \return int
    */
    int targetBuildNumber() const;
    /*!
     \brief Return last update date.

     \return QDateTime
    */
    QDateTime lastUpdateDate() const;
    /*!
     \brief Return last check date.

     \return QDateTime
    */
    QDateTime lastCheckDate() const;

    /*!
     \brief Check for update.

    */
    Q_INVOKABLE void checkForUpdate();
    /*!
     \brief Download update.

    */
    Q_INVOKABLE void downloadUpdate();
    /*!
     \brief Force download on GSM.

    */
    Q_INVOKABLE void forceAllowGSMDownload();
    /*!
     \brief Apply update.

    */
    Q_INVOKABLE void applyUpdate();
    /*!
     \brief Cancel update.

    */
    Q_INVOKABLE void cancelUpdate();
    /*!
     \brief Pause download.

    */
    Q_INVOKABLE void pauseDownload();
    /*!
     \brief
     TODO: Isn't this redundant now?

    */
    Q_INVOKABLE void checkTarget();
    /*!
     \brief Production reset.

    */
    Q_INVOKABLE void productionReset();
    /*!
     \brief Factory reset.

    */
    Q_INVOKABLE void factoryReset();
    /*!
     \brief Return whether or not target build number is newer than current build number.

     \return bool
    */
    Q_INVOKABLE bool getIsTargetNewer() const;

signals:
    /*!
     \brief Current build number changed.

    */
    void currentBuildNumberChanged();
    /*!
     \brief Device name changed.

    */
    void deviceNameChanged();
    /*!
     \brief Channel name changed.

    */
    void channelNameChanged();
    /*!
     \brief Current Ubuntu build number changed.

    */
    void currentUbuntuBuildNumberChanged();
    /*!
     \brief Current device build number changed.

    */
    void currentDeviceBuildNumberChanged();
    /*!
     \brief Current custom build number changed.

    */
    void currentCustomBuildNumberChanged();
    /*!
     \brief Target build number changed.

    */
    void targetBuildNumberChanged();
    /*!
     \brief Detailed map of version details changed.

    */
    void detailedVersionDetailsChanged();
    /*!
     \brief Last update date changed.

    */
    void lastUpdateDateChanged();
    /*!
     \brief Last check date changed.

    */
    void lastCheckDateChanged();

    /*!
     \brief Download mode changed.

    */
    void downloadModeChanged();
    /*!
     \brief An update was not found.

    */
    void updateNotFound();
    /*!
     \brief The update process failed.

     \param reason
    */
    void updateProcessFailed(const QString &reason);

    /*!
     \brief System is rebooting.
     Note: It's not certain this signal will be emitted before the system reboots.

     \param status
    */
    void rebooting(const bool status);
    /*!
     \brief Update failed.

     \param consecutiveFailureCount
     \param lastReason
    */
    void updateFailed(const int &consecutiveFailureCount, const QString &lastReason);
    /*!
     \brief Update downloaded.

    */
    void updateDownloaded();
    /*!
     \brief Update download started.

    */
    void downloadStarted();
    /*!
     \brief Update paused.

     \param percentage
    */
    void updatePaused(const int &percentage);
    /*!
     \brief

     \param isAvailable
     \param downloading
     \param availableVersion
     \param updateSize
     \param lastUpdateDate
     \param errorReason
    */
    void updateAvailableStatus(const bool isAvailable,
                               const bool downloading,
                               const QString &availableVersion,
                               const int &updateSize,
                               const QString &lastUpdateDate,
                               const QString &errorReason);
    /*!
     \brief Handle availability status changes for updates.

     \param percentage
     \param eta
    */
    void updateProgress(const int &percentage, const double &eta);

private slots:
    /*!
     \brief Gracefully handle ownership changes.

     \param name of service
     \param old owner
     \param new owner
    */
    void slotNameOwnerChanged(const QString&, const QString&, const QString&);
    /*!
     \brief Handle setting changes in SI.

     \param key the key that changed
     \param newvalue of the key that changed
    */
    void settingsChanged(const QString &key, const QString &newvalue);

private:
    int m_currentBuildNumber; /*!< The current build number. */
    QMap<QString, QVariant> m_detailedVersion; /*!< A map of detailed version information. */
    QDateTime m_lastUpdateDate; /*!< The last update date. */
    int m_downloadMode; /*!< Current download mode.*/
    QDBusConnection m_systemBusConnection; /*!< Holds the system bus connection. */
    QDBusServiceWatcher m_serviceWatcher; /*!< Service watcher for when the SI disappears from the bus. */
    QDBusInterface m_systemServiceIface; /*!< The interface proxy for SI. */

    QDateTime m_lastCheckDate; /*!< Last check date. */
    QString m_channelName; /*!< Channel name (e.g. ubuntu/rc-proposed/meizu.en). */
    int m_targetBuildNumber; /*!< Target build number. */
    QString m_objectPath; /*!< SI object path (/Service) */
    QString m_deviceName; /*!< Device name (e.g. mako). */

    /*!
     \brief Initializes the properties we want to store, using a sync call to the system image dbus.

    */
    void initializeProperties();
    /*!
     \brief Sets up connections on the interface.

    */
    void setUpInterface();
};

#endif // QSYSTEMIMAGE_H
