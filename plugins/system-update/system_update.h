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

public:
    explicit SystemUpdate(QObject *parent = 0);
    ~SystemUpdate();

    int downloadMode();
    void setDownloadMode(int);
    QString lastUpdateDate();
    int currentBuildNumber();
    QString currentUbuntuBuildNumber();
    QString currentDeviceBuildNumber();

    void checkForUpdate();
    void downloadUpdate();
    void applyUpdate();
    void cancelUpdate();
    void pauseDownload();

public Q_SLOTS:
    void ProcessAvailableStatus(bool, bool, QString, int, QString, QString);
    void ProcessSettingChanged(QString, QString);

Q_SIGNALS:
    void updateAvailable(const QString& packageName, Update *update);
    void updateNotFound();
    void updateProgress(int percentage, double eta);
    void updatePaused(int percentage);
    void updateDownloaded();
    void updateFailed(int consecutiveFailureCount, QString lastReason);
    void downloadModeChanged();
    void versionChanged();
    void updateProcessFailed(const QString& reason);

private Q_SLOTS:
    void updateDownloadProgress(int percentage, double eta);

private:
    int m_currentBuildNumber;
    QMap<QString, QString> m_detailedVersion;
    QString m_lastUpdateDate;
    int m_downloadMode;

    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_SystemServiceIface;
    Update *update;

    void setCurrentDetailedVersion();
};

}

#endif // SYSTEMUPDATE_H
