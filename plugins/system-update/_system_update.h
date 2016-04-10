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

#ifndef SYSTEMUPDATE_H
#define SYSTEMUPDATE_H

#include <QtDBus>
#include <QDBusInterface>
#include <QObject>
#include <QProcess>
#include <QUrl>

#include "update.h"

#define UBUNTU_PACKAGE_NAME "UbuntuImage"

namespace UpdatePlugin {

class SystemUpdate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UpdateManager::UpdateStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(int downloadMode READ downloadMode WRITE setDownloadMode
               NOTIFY downloadModeChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(int size READ size NOTIFY sizeChanged)

public:
    explicit SystemUpdate(QObject *parent = 0);
    ~SystemUpdate();

    const int downloadMode();
    void setDownloadMode(const int &downloadMode);

    const UpdateManager::UpdateStatus status();

    const QString version();
    const double progress();
    const int size();

    // Q_INVOKABLE void checkForUpdate();
    Q_INVOKABLE void downloadUpdate();
    Q_INVOKABLE void forceAllowGSMDownload();
    Q_INVOKABLE void applyUpdate();
    Q_INVOKABLE void cancelUpdate();
    Q_INVOKABLE void pauseDownload();
    Q_INVOKABLE void checkTarget();

public Q_SLOTS:
    void ProcessAvailableStatus(bool, bool, QString, int, QString, QString);
    void ProcessSettingChanged(QString, QString);

Q_SIGNALS:
    void downloadModeChanged();
    void versionChanged();
    void statusChanged();
    void progressChanged();
    void sizeChanged();
    // void updateAvailable(const QString& packageName);
    // void updateNotFound();
    // void updateProgress(int percentage, double eta);
    // void updatePaused(int percentage);
    // void downloadStarted();
    // void updateDownloaded();
    // void updateFailed(int consecutiveFailureCount, QString lastReason);
    // void updateProcessFailed(const QString& reason);
    // void rebooting(bool status);

private Q_SLOTS:
    void updateDownloadProgress(int percentage, double eta);

private:
    QDateTime lastUpdateDate();
    int currentBuildNumber();
    QString currentUbuntuBuildNumber();
    QString currentDeviceBuildNumber();
    QString currentCustomBuildNumber();
    QMap<QString, QVariant> detailedVersionDetails();
    QString deviceName();

    int m_currentBuildNumber;
    QMap<QString, QVariant> m_detailedVersion;
    QDateTime m_lastUpdateDate;
    int m_downloadMode;
    QString m_deviceName;

    UpdateManager::UpdateStatus m_status;

    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_SystemServiceIface;

    void setCurrentDetailedVersion();
};

}

#endif // SYSTEMUPDATE_H
