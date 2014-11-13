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

#include <QCoreApplication>
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
    void up_connect();
    void up_disconnect();
    UpClient *m_client;
    QCoreApplication *m_app;
    gulong m_addedHandler, m_removedHandler;

private Q_SLOTS:
    void onApplicationStateChanged(Qt::ApplicationState st);
};

void deviceChanged(UpClient *client,
                   GParamSpec *pspec G_GNUC_UNUSED,
                   gpointer user_data)
{
    BatteryItem *item (static_cast<BatteryItem *> (user_data));

#if !UP_CHECK_VERSION(0, 99, 0)
    gboolean ret = up_client_enumerate_devices_sync (client, nullptr, nullptr);
    if (!ret) {
        item->setVisibility (false);
    } else
#endif
    {
        GPtrArray *devices = up_client_get_devices (client);
        item->setVisibility (devices->len > 0);
        g_ptr_array_unref (devices);
    }
}

BatteryItem::BatteryItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent),
    m_client(up_client_new()),
    m_app(QCoreApplication::instance()),
    m_addedHandler(0),
    m_removedHandler(0)
{
    deviceChanged(m_client, nullptr, this);
    connect(m_app, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));
    up_connect();
}

void BatteryItem::onApplicationStateChanged(Qt::ApplicationState st)
{
    if (st == Qt::ApplicationActive)
        up_connect();
    else
        up_disconnect();
}

void BatteryItem::up_connect()
{
    if (!m_addedHandler) {
        m_addedHandler = g_signal_connect (m_client,
                                          "device-added",
                                          G_CALLBACK (::deviceChanged),
                                          this /* user_data */);
    }

    if (!m_removedHandler) {
        m_removedHandler = g_signal_connect (m_client,
                                            "device-removed",
                                            G_CALLBACK (::deviceChanged),
                                            this /* user_data */);
    }
}

void BatteryItem::up_disconnect()
{
    if (m_addedHandler) {
        g_signal_handler_disconnect (m_client, m_addedHandler);
        m_addedHandler = 0;
    }

    if (m_removedHandler) {
        g_signal_handler_disconnect (m_client, m_removedHandler);
        m_removedHandler = 0;
    }
}

void BatteryItem::setVisibility(bool visible)
{
    setVisible(visible);
}

BatteryItem::~BatteryItem()
{
    up_disconnect();
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
