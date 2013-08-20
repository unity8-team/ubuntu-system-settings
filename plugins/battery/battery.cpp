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
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#include "battery.h"
#include <glib.h>
#include <libupower-glib/upower.h>
#include <QEvent>
#include <QDBusReply>
#include <QtCore/QDebug>

Battery::Battery(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_powerdIface ("com.canonical.powerd",
                   "/com/canonical/powerd",
                   "com.canonical.powerd",
                   m_systemBusConnection),
    m_deviceString("")
{
    m_device = up_device_new();

    buildDeviceString();
    getLastFullCharge();

    if (!m_powerdIface.isValid()) {
        m_powerdRunning = false;
        return;
    }
    else
        m_powerdRunning = true;
}

bool Battery::powerdRunning()
{
    return m_powerdRunning;
}

void Battery::buildDeviceString() {
    UpClient *client;
    gboolean returnIsOk;
    GPtrArray *devices;
    UpDevice *device;
    UpDeviceKind kind;

    client = up_client_new();
    returnIsOk = up_client_enumerate_devices_sync(client, NULL, NULL);

    if(!returnIsOk)
        return;

    devices = up_client_get_devices(client);

    for (uint i=0; i < devices->len; i++) {
        device = (UpDevice *)g_ptr_array_index(devices, i);
        g_object_get(device, "kind", &kind, NULL);
        if (kind == UP_DEVICE_KIND_BATTERY) {
            m_deviceString = QString(up_device_get_object_path(device));
        }
    }

    g_ptr_array_unref(devices);
    g_object_unref(client);
}

QString Battery::deviceString()
{
    return m_deviceString;
}

int Battery::lastFullCharge()
{
    return m_lastFullCharge;
}

void Battery::getLastFullCharge()
{
    UpHistoryItem *item;
    GPtrArray *values;
    gint32 offset = 0;
    GTimeVal timeval;

    g_get_current_time(&timeval);
    offset = timeval.tv_sec;
    up_device_set_object_path_sync(m_device, m_deviceString.toStdString().c_str(), NULL, NULL);
    values = up_device_get_history_sync(m_device, "charge", 864000, 1000, NULL, NULL);
    for (uint i=values->len-1; i > 0; i--) {
        item = (UpHistoryItem *) g_ptr_array_index(values, i);

        if (up_history_item_get_state(item) == UP_DEVICE_STATE_FULLY_CHARGED) {
            m_lastFullCharge = (int)((offset - (gint32) up_history_item_get_time(item)));
            Q_EMIT(lastFullChargeChanged());
            return;
        }
    }
}

/* TODO: refresh values over time for dynamic update */
QVariantList Battery::getHistory(const QString &deviceString, const int timespan, const int resolution)
{
    UpHistoryItem *item;
    GPtrArray *values;
    gint32 offset = 0;
    GTimeVal timeval;
    QVariantList listValues;

    g_get_current_time(&timeval);
    offset = timeval.tv_sec;
    up_device_set_object_path_sync(m_device, deviceString.toStdString().c_str(), NULL, NULL);
    values = up_device_get_history_sync(m_device, "charge", timespan, resolution, NULL, NULL);
    for (uint i=0; i < values->len; i++) {
        QVariantMap listItem;
        item = (UpHistoryItem *) g_ptr_array_index(values, i);

        if (up_history_item_get_state(item) == UP_DEVICE_STATE_UNKNOWN)
            continue;

        if (up_history_item_get_state(item) == UP_DEVICE_STATE_FULLY_CHARGED) {
            m_lastFullCharge = (int)(offset - ((gint32) up_history_item_get_time(item)));
            Q_EMIT(lastFullChargeChanged());
        }

        listItem.insert("time",(offset - (gint32) up_history_item_get_time(item)));
        listItem.insert("value",up_history_item_get_value(item));
        listValues += listItem;
    }
    return listValues;
}

Battery::~Battery() {
    g_object_unref(m_device);
}
