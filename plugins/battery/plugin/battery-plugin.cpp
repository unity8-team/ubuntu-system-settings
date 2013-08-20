/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#include "battery-plugin.h"

#include <QDebug>
#include <QStringList>
#include <SystemSettings/ItemBase>

#include <glib.h>
#include <libupower-glib/upower.h>

using namespace SystemSettings;

class BatteryItem: public ItemBase
{
    Q_OBJECT

public:
    BatteryItem(const QVariantMap &staticData, QObject *parent = 0);
    ~BatteryItem();
};

BatteryItem::BatteryItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    UpClient *client = up_client_new();
    gboolean ret = up_client_enumerate_devices_sync(client, NULL, NULL);
    if (!ret) {
        setVisible(false);
    } else {
        GPtrArray *devices = up_client_get_devices(client);
        setVisible (devices->len > 0);
        g_ptr_array_unref (devices);
    }

    g_object_unref (client);
}

BatteryItem::~BatteryItem()
{
}

BatteryPlugin::BatteryPlugin():
    QObject()
{
}


ItemBase *BatteryPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new BatteryItem(staticData, parent);
}

#include "battery-plugin.moc"
