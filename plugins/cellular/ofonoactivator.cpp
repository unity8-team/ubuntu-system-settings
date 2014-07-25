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

#include "ofonoactivator.h"

#include"nm_manager_proxy.h"
#include"nm_settings_proxy.h"
#include"nm_settings_connection_proxy.h"
#include<QCoreApplication>

typedef QMap<QString,QVariantMap> Vardict;
Q_DECLARE_METATYPE(Vardict)

namespace {

QString nmService("org.freedesktop.NetworkManager");
QString nmSettingsPath("/org/freedesktop/NetworkManager/Settings");
QString nmPath("/org/freedesktop/NetworkManager");

QDBusObjectPath detectConnection(const QString &ofonoContext) {
    auto ofonoContextBase = ofonoContext.split('/').back();

    OrgFreedesktopNetworkManagerSettingsInterface settings(nmService, nmSettingsPath,
            QDBusConnection::systemBus());
    auto reply = settings.ListConnections();
    reply.waitForFinished();
    if(!reply.isValid()) {
        qWarning() << "Error getting connection list: " << reply.error().message() << "\n";
    }
    auto connections = reply.value(); // Empty list if failed.

    for(const auto &c : connections) {
        OrgFreedesktopNetworkManagerSettingsConnectionInterface connProxy(nmService,
                c.path(), QDBusConnection::systemBus());
        auto reply2 = connProxy.GetSettings();
        reply2.waitForFinished();
        if(!reply2.isValid()) {
            qWarning() << "Error getting property: " << reply2.error().message() << "\n";
            continue;
        }
        auto settings = reply2.value();
        auto context = settings["connection"]["id"].toString();
        auto contextBase = context.split('/').back();
        if(contextBase == ofonoContextBase) {
            return c;
        }
    }
    return QDBusObjectPath("");
}

QDBusObjectPath detectDevice(const QString &ofonoContext) {
    auto expectedIface = QString("/") + ofonoContext.split('/')[1];
    OrgFreedesktopNetworkManagerInterface nm(nmService, nmPath, QDBusConnection::systemBus());
    auto reply = nm.GetDevices();
    reply.waitForFinished();
    auto devices = reply.value();

    for(const auto &device : devices) {
        QDBusInterface iface(nmService, device.path(), "org.freedesktop.DBus.Properties",
                QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> ifaceReply = iface.call("Get",
                "org.freedesktop.NetworkManager.Device", "Interface");
        if(!ifaceReply.isValid()) {
            qWarning() << "Error getting property: " << ifaceReply.error().message() << "\n";
            continue;
        }
        auto devIface = ifaceReply.value().variant().toString();
        if(devIface == expectedIface) {
            return device;
        }
    }
    return QDBusObjectPath("");
}

void activateOfono(QDBusObjectPath connection, QDBusObjectPath device) {
    OrgFreedesktopNetworkManagerInterface nm(nmService, nmPath, QDBusConnection::systemBus());
    nm.ActivateConnection(connection, device, QDBusObjectPath("/"));
}

}

OfonoActivator::OfonoActivator(QObject *parent) : QObject(parent) {
    static bool isRegistered = false;
    if(!isRegistered) {
        qDBusRegisterMetaType<Vardict>();
        isRegistered = true;
    }
}

Q_INVOKABLE bool OfonoActivator::activate(const QString ofonoContext) {
    printf("Activating ofono context: %s\n", ofonoContext.toUtf8().data());
    auto conn = detectConnection(ofonoContext);
    if(conn.path() == "") {
        qWarning() << "Could not detect connection object to use for Ofono activation.\n";
        return false;
    }
    auto dev = detectDevice(ofonoContext);
    if(dev.path() == "") {
        qWarning() << "Could not detect device object to use for Ofono activation.\n";
        return false;
    }
    activateOfono(conn, dev);
    return true;
}
