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

WifiDbusHelper::WifiDbusHelper(QObject *parent) : QObject(parent) {
}

void WifiDbusHelper::connect(QString ssid, int security, QString password) {
    const QString nm_name("com.something");
    const QString nm_interface("com.something");
    const QString nm_path("/com/something");
    QDBusInterface service(nm_name, nm_path, nm_interface, QDBusConnection::systemBus());
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
    const QString service("org.freedesktop.NetworkManager");
    const QString object("/org/freedesktop/NetworkManager/Settings");
    const QString baseInterface("org.freedesktop.NetworkManager.Settings");
    const QString connectionInterface("org.freedesktop.NetworkManager.Settings.Connection");
    QDBusInterface iface(service, object, baseInterface, QDBusConnection::systemBus());
    //QDBusReply<QStringList> listResult = iface.call("ListConnections");
    QDBusReply<QList<QDBusObjectPath> > listResult = iface.call("ListConnections");
    if(!listResult.isValid()) {
        qDebug() << "Could not query network list: " << listResult.error() << "\n";
        return networks;
    }
    for(const auto &i : listResult.value()) {
        QDBusInterface connIface(service, i.path(), connectionInterface, QDBusConnection::systemBus());
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
                    networks.push_back(QPair<QString, QString>(id->toString(), i.path()));
                }
            }
        }
    }
    return networks;
}
