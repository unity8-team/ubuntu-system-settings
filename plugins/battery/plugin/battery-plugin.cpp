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
    explicit BatteryItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);
    ~BatteryItem();

private:
    UpClient *m_client;
    gulong m_addedHandler, m_removedHandler;
};

void deviceChanged(UpClient *client,
                   GParamSpec *pspec G_GNUC_UNUSED,
                   gpointer user_data)
{
    BatteryItem *item (static_cast<BatteryItem *> (user_data));

    gboolean ret = up_client_enumerate_devices_sync (client, nullptr, nullptr);
    if (!ret) {
        item->setVisibility (false);
    } else {
        GPtrArray *devices = up_client_get_devices (client);
        item->setVisibility (devices->len > 0);
        g_ptr_array_unref (devices);
    }

}

BatteryItem::BatteryItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent),
    m_client(up_client_new()),
    m_addedHandler(0),
    m_removedHandler(0)
{
    deviceChanged(m_client, nullptr, this);
    m_addedHandler = g_signal_connect (m_client,
                                      "device-added",
                                      G_CALLBACK (::deviceChanged),
                                      this /* user_data */);
    m_removedHandler = g_signal_connect (m_client,
                                        "device-removed",
                                        G_CALLBACK (::deviceChanged),
                                        this /* user_data */);
}

void BatteryItem::setVisibility(bool visible)
{
    setVisible(visible);
}

BatteryItem::~BatteryItem()
{
    if (m_addedHandler) {
        g_signal_handler_disconnect (m_client, m_addedHandler);
        m_addedHandler = 0;
    }

    if (m_removedHandler) {
        g_signal_handler_disconnect (m_client, m_removedHandler);
        m_removedHandler = 0;
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
