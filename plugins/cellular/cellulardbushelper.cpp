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

#include "nm_manager_proxy.h"
#include "cellulardbushelper.h"
#include <QStringList>
#include <QDBusReply>
#include <QtDebug>
#include <QDBusInterface>
#include <QDBusMetaType>

typedef QMap<QString, QVariantMap> nmConnectionArg;
Q_DECLARE_METATYPE(nmConnectionArg)

#define NM_SERVICE_NAME "org.freedesktop.NetworkManager"
#define NM_DBUS_PATH "/org/freedesktop/NetworkManager"
#define NM_DEVICE_INTERFACE "org.freedesktop.NetworkManager.Device"

namespace {

const QString nm_service("org.freedesktop.NetworkManager");
const QString nm_object("/org/freedesktop/NetworkManager/Settings");
const QString settings_interface("org.freedesktop.NetworkManager.Settings");
const QString connection_interface("org.freedesktop.NetworkManager.Settings.Connection");

void enableHotspot(const QString &ssid, const QString &password,
        const QDBusObjectPath &device, const QDBusObjectPath &specific) {
    nmConnectionArg connection;

    QVariantMap wireless;
    wireless[QString("security")] = QVariant(QString("802-11-wireless-security"));
    wireless[QString("ssid")] = QVariant(ssid.toUtf8());
    wireless[QString("mode")] = QVariant(QString("adhoc"));
    connection["802-11-wireless"] = wireless;

    QVariantMap connsettings;
    connsettings[QString("autoconnect")] = QVariant(false);
    connsettings[QString("uuid")] = QVariant(QString("aab22b5d-7342-48dc-8920-1b7da31d6829"));
    connsettings[QString("type")] = QVariant(QString("802-11-wireless"));
    connection["connection"] = connsettings;

    QVariantMap ipv4;
    ipv4[QString("addressess")] = QVariant(QStringList());
    ipv4[QString("dns")] = QVariant(QStringList());
    ipv4[QString("method")] = QVariant(QString("shared"));
    ipv4[QString("routes")] = QVariant(QStringList());
    connection["ipv4"] = ipv4;

    QVariantMap security;
    security[QString("proto")] = QVariant(QStringList{"rsn"});
    security[QString("pairwise")] = QVariant(QStringList{"ccmp"});
    security[QString("group")] = QVariant(QStringList{"ccmp"});
    security[QString("key-mgmt")] = QVariant(QString("wpa-psk"));
    security[QString("psk")] = QVariant(password);
    connection["802-11-wireless-security"] = security;

    OrgFreedesktopNetworkManagerInterface mgr(NM_SERVICE_NAME,
            NM_DBUS_PATH, QDBusConnection::systemBus());
    auto reply = mgr.AddAndActivateConnection(connection, device, specific);
    reply.waitForFinished();
    if(!reply.isValid()) {
        qDebug() << "Creating hotspot failed: " << reply.error().message() << "\n";
    } else {
        qDebug() << "Successfully created wifi hotspot.\n";
    }
}

void disableHotspot(const QDBusObjectPath &path) {
    QDBusInterface device(NM_SERVICE_NAME, path.path(), NM_DEVICE_INTERFACE);
    QDBusReply<void> reply = device.call("Disconnect");
    if(!reply.isValid()) {
        qDebug() << "Disconnecting hotspot failed: " << reply.error().message() << "\n";
    }
}

}



CellularDbusHelper::CellularDbusHelper(QObject *parent) : QObject(parent) {
    static bool isRegistered = false;
    if(!isRegistered) {
        qDBusRegisterMetaType<nmConnectionArg>();
        isRegistered = true;
    }
}

QString CellularDbusHelper::getHotspotName() {
    return "Ubuntu hotspot";
}

QString CellularDbusHelper::getHotspotPassword() {
    return "qwerty0";
}

void CellularDbusHelper::setHotspotSettings(QString ssid_, QString password_) {
    ssid = ssid_;
    password = password_;
}

bool CellularDbusHelper::isHotspotActive() {
    return false;
}

void CellularDbusHelper::toggleHotspot(bool on) {
    QDBusObjectPath device("/org/freedesktop/NetworkManager/Devices/0");
    QDBusObjectPath specific("/");
    if(on) {
        enableHotspot(ssid, password, device, specific);
    } else {
        printf("Toggling hotspot off.\n");
    }
}
