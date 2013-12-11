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
    void setVisibility(bool visible);
    ~BatteryItem();

private:
    UpClient *m_client;
    gulong added_handler, removed_handler;
};

void deviceChanged(UpClient *client,
                   GParamSpec *pspec G_GNUC_UNUSED,
                   gpointer user_data)
{
    BatteryItem *item (static_cast<BatteryItem *> (user_data));
    g_object_ref (client);

    gboolean ret = up_client_enumerate_devices_sync (client, NULL, NULL);
    if (!ret) {
        item->setVisibility (false);
    } else {
        GPtrArray *devices = up_client_get_devices (client);
        item->setVisibility (devices->len > 0);
        g_ptr_array_unref (devices);
    }

    g_object_unref (client);

}

BatteryItem::BatteryItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent),
    m_client(up_client_new()),
    added_handler(0),
    removed_handler(0)
{
    added_handler = g_signal_connect (m_client,
                                      "device-added",
                                      G_CALLBACK (::deviceChanged),
                                      this /* user_data */);
    removed_handler = g_signal_connect (m_client,
                                        "device-removed",
                                        G_CALLBACK (::deviceChanged),
                                        this /* user_data */);
    deviceChanged(m_client, NULL, this);
}

void BatteryItem::setVisibility(bool visible)
{
    setVisible(visible);
}

BatteryItem::~BatteryItem()
{
    if (added_handler) {
        g_signal_handler_disconnect (m_client, added_handler);
        added_handler = 0;

    if (removed_handler) {
        g_signal_handler_disconnect (m_client, removed_handler);
        removed_handler = 0;
    }

    g_object_unref (m_client);
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
