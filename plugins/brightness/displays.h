/*
 * Copyright (C) 2015 Canonical Ltd
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
 */

#ifndef DISPLAYS_H
#define DISPLAYS_H

#include <QDBusInterface>
#include <QObject>

class Displays : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QStringList displays
                READ displays
                NOTIFY displaysChanged )

public:
    explicit Displays(QObject *parent = 0);
    ~Displays();

    QStringList displays() const;

Q_SIGNALS:
    void displaysChanged(QStringList &displays);

private:
    QDBusConnection m_systemBusConnection;
    QDBusInterface m_unityInterface;
    QStringList m_displays;
};

#endif // DISPLAYS_H
