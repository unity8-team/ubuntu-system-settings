/*
 * Copyright (C) 2013 Canonical Ltd
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
 *
*/

#ifndef UPDATE_H
#define UPDATE_H

#include <QDBusInterface>
#include <QObject>
#include <QProcess>
#include <QUrl>

class Update : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString OSVersion
                READ OSVersion
                CONSTANT)
    Q_PROPERTY( int updateAvailable
                READ UpdateAvailable
                NOTIFY updateAvailableChanged )
    Q_PROPERTY( QString updateVersion
                READ UpdateVersion
                NOTIFY updateAvailableChanged)
    Q_PROPERTY( QString updateSize
                READ UpdateSize
                NOTIFY updateAvailableChanged)
    Q_PROPERTY( QString updateDescriptions
                READ UpdateDescriptions
                NOTIFY updateAvailableChanged)

    
public:
    explicit Update(QObject *parent = 0);
    ~Update();

    QString OSVersion();

    int UpdateAvailable();
    QString UpdateVersion();
    QString UpdateSize();
    QString UpdateDescriptions();

    Q_INVOKABLE void TriggerUpdate();
    Q_INVOKABLE void CancelUpdate();
    Q_INVOKABLE void Reboot();


public Q_SLOTS:
    bool slotUpdateAvailableStatus(bool pendingUpdate);

Q_SIGNALS:
    bool updateAvailableChanged();
    void readyToReboot();
    void updateFailed();
    void updateCanceled();

private:
    QString m_OSVersion;

    int m_updateAvailable;
    QString m_updateVersion;
    QString m_updateSize;
    QString m_updateDescriptions;

    QDBusConnection m_sessionBusConnection;
    QString m_objectPath;
    QDBusInterface m_SystemServiceIface;

    void m_getUpdateInfos();
};

#endif // UPDATE_H
