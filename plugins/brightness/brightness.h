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

#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <QDBusInterface>
#include <QObject>

class Brightness : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool powerdRunning
                READ getPowerdRunning
                CONSTANT)
    Q_PROPERTY (bool autoBrightnessAvailable
                READ getAutoBrightnessAvailable
                CONSTANT)

public:
    explicit Brightness(QObject *parent = 0);
    bool getPowerdRunning() const;
    bool getAutoBrightnessAvailable() const;

private:
    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_powerdIface;
    bool m_powerdRunning;
    bool m_autoBrightnessAvailable;
};

#endif // BRIGHTNESS_H
