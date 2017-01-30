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

#include "displays/displaymodel.h"
#include "displays/mirclient.h"

#include <QAbstractItemModel>
#include <QDBusInterface>
#include <QObject>

class Q_DECL_EXPORT Brightness : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool powerdRunning
                READ getPowerdRunning
                CONSTANT)
    Q_PROPERTY (bool autoBrightnessAvailable
                READ getAutoBrightnessAvailable
                CONSTANT)
    Q_PROPERTY (bool widiSupported
                READ getWidiSupported
                CONSTANT)
    Q_PROPERTY (QAbstractItemModel* allDisplays
                READ allDisplays
                CONSTANT)
    Q_PROPERTY (QAbstractItemModel* changedDisplays
                READ changedDisplays
                CONSTANT)
    Q_PROPERTY (QAbstractItemModel* connectedDisplays
                READ connectedDisplays
                CONSTANT)

public:
    explicit Brightness(QObject *parent = Q_NULLPTR);
    explicit Brightness(QDBusConnection dbus,
                        DisplayPlugin::MirClient *mirClient,
                        QObject *parent = Q_NULLPTR);
    bool getPowerdRunning() const;
    bool getAutoBrightnessAvailable() const;
    bool getWidiSupported() const;
    QAbstractItemModel* allDisplays();
    QAbstractItemModel* changedDisplays();
    QAbstractItemModel* connectedDisplays();
    Q_INVOKABLE void applyDisplayConfiguration();

private:
    QDBusConnection m_systemBusConnection;
    DisplayPlugin::MirClient *m_mirClient = Q_NULLPTR;
    QDBusInterface m_powerdIface;
    QString m_objectPath;
    bool m_powerdRunning;
    bool m_autoBrightnessAvailable;
    DisplayPlugin::DisplayModel m_displays;
    DisplayPlugin::DisplaysFilter m_changedDisplays;
    DisplayPlugin::DisplaysFilter m_connectedDisplays;

private slots:
    void refreshMirDisplays();
};

#endif // BRIGHTNESS_H
