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
 * Iain Lane <iain.lane@canonical.com>
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
    Q_PROPERTY( QString versionOS
                READ versionOS
                CONSTANT)
    Q_PROPERTY( int updateAvailable
                READ getUpdateAvailable
                NOTIFY updateAvailableChanged )
    
public:
    explicit Update(QObject *parent = 0);
    ~Update();
    QString versionOS();
    int getUpdateAvailable();

public Q_SLOTS:
    bool slotUpdateAvailableStatus(bool pendingUpdate);

Q_SIGNALS:
    bool updateAvailableChanged();

private:
    int m_updateAvailable;
    QString m_versionOS;
    QDBusConnection m_sessionBusConnection;
    QString m_objectPath;
    QDBusInterface m_SystemServiceIface;
};

#endif // UPDATE_H
