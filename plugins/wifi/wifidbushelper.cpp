/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wifidbushelper.h"
#include <QStringList>
#include <QDBusReply>
#include <QtDebug>

#define DBUS_SERVICE_NAME "com.something"
#define DBUS_INTERFACE "com.something"
#define DBUS_PATH "/com/something"

WifiDbusHelper::WifiDbusHelper(QObject *parent) : QObject(parent)/*,
    service(DBUS_SERVICE_NAME, DBUS_PATH, DBUS_INTERFACE)*/ {
}

void WifiDbusHelper::connect(QString ssid, int security, QString password) {
    // Convert security enum to NM flags here.
    printf("Connecting to %s, security %d, password %s.\n",
            ssid.toUtf8().data(), security, password.toUtf8().data());
/*
    QDBusReply<unsigned int> result = service.call("XXX",
            ssid, securityFlags, password);
 */
}
