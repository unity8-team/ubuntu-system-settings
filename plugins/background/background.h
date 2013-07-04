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

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QDBusInterface>
#include <QObject>
#include <QProcess>

class Background : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString backgroundFile
                READ backgroundFile
                NOTIFY backgroundFileChanged )
    
public:
    explicit Background(QObject *parent = 0);
    ~Background();
    QString backgroundFile();

public Q_SLOTS:
    void slotChanged();

Q_SIGNALS:
    void backgroundFileChanged();

private:
    QString m_backgroundFile;
    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_accountsserviceIface;
    QString getBackgroundFile();
};

#endif // BACKGROUND_H
