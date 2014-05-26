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
#include <QDBusInterface>

namespace {

const QString nm_service("org.freedesktop.NetworkManager");
const QString nm_object("/org/freedesktop/NetworkManager/Settings");
const QString settings_interface("org.freedesktop.NetworkManager.Settings");
const QString connection_interface("org.freedesktop.NetworkManager.Settings.Connection");

}

WifiDbusHelper::WifiDbusHelper(QObject *parent) : QObject(parent) {
}

void WifiDbusHelper::connect(QString ssid, int security, QString password) {
    QDBusInterface service(nm_service, nm_object, settings_interface, QDBusConnection::systemBus());
    printf("Connecting to %s, security %d, password %s.\n",
            ssid.toUtf8().data(), security, password.toUtf8().data());
/*
    // Convert security enum to NM flags here.
    QDBusReply<unsigned int> result = service.call("XXX",
            ssid, securityFlags, password);
 */
}

QList<QPair<QString, QString>> WifiDbusHelper::getPreviouslyConnectedWifiNetworks() {
    QList<QPair<QString, QString>> networks;
    const QString wifikey("802-11-wireless");
    const QString idkey("id");
    QDBusInterface iface(nm_service, nm_object, settings_interface, QDBusConnection::systemBus());
    //QDBusReply<QStringList> listResult = iface.call("ListConnections");
    QDBusReply<QList<QDBusObjectPath> > listResult = iface.call("ListConnections");
    if(!listResult.isValid()) {
        qDebug() << "Could not query network list: " << listResult.error() << "\n";
        return networks;
    }
    for(const auto &i : listResult.value()) {
        QDBusInterface connIface(nm_service, i.path(), connection_interface, QDBusConnection::systemBus());
        auto replymsg = connIface.call("GetSettings");
        if(replymsg.type() != QDBusMessage::ReplyMessage) {
            printf("Reply is incorrect.\n");
            return networks;
        }
        auto args = replymsg.arguments();
        if(args.size() != 1) {
            qDebug() << "DBus reply is malformed: " << replymsg.errorMessage() << "\n";
            return networks;
        }
        QDBusArgument r = args[0].value<QDBusArgument>();
        QMap<QString, QVariant> m;
        r >> m;
        if(m.find(wifikey) != m.end()) {
            auto id = m.find(idkey);
            if(id == m.end()) {
                qDebug() << "NM object missing required id field.\n";
            } else {
                if(id->type() != QVariant::String) {
                    qDebug() << "NM object id is malformed.\n";
                } else {
                    // connection -> timestamp
                    networks.push_back(QPair<QString, QString>(id->toString(), i.path()));
                }
            }
        }
    }
    return networks;
}

void WifiDbusHelper::forgetConnection(const QString dbus_path) {
    QDBusInterface service(nm_service, dbus_path, connection_interface, QDBusConnection::systemBus());
    auto reply = service.call("Delete");
    if(reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Error forgetting connection: " << reply.errorMessage() << "\n";
    }
}
