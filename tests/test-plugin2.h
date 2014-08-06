/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SYSTEM_SETTINGS_TEST_PLUGIN2_H
#define SYSTEM_SETTINGS_TEST_PLUGIN2_H

#include <QObject>
#include <SystemSettings/PluginInterface>

class TestPlugin2: public QObject, public SystemSettings::PluginInterface2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.ubuntu.SystemSettings.PluginInterface/2.0")
    Q_INTERFACES(SystemSettings::PluginInterface2)

public:
    TestPlugin2();

    SystemSettings::ItemBase *createItem(const QVariantMap &staticData,
                                         QObject *parent = 0);
    bool reset();
};

#endif // SYSTEM_SETTINGS_TEST_PLUGIN2_H
