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
#include "nm_settings_proxy.h"
#include "nm_settings_connection_proxy.h"
#include "cellulardbushelper.h"
#include <QStringList>
#include <QDBusReply>
#include <QtDebug>
#include <QDBusInterface>
#include <QDBusMetaType>

typedef QMap<QString, QVariantMap> nmConnectionArg;
Q_DECLARE_METATYPE(nmConnectionArg)

namespace {

const QString nm_service("org.freedesktop.NetworkManager");
const QString nm_settings_object("/org/freedesktop/NetworkManager/Settings");
const QString nm_settings_interface("org.freedesktop.NetworkManager.Settings");
const QString nm_connection_interface("org.freedesktop.NetworkManager.Settings.Connection");
const QString nm_dbus_path("/org/freedesktop/NetworkManager");
const QString nm_device_interface("org.freedesktop.NetworkManager.Device");

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

    OrgFreedesktopNetworkManagerInterface mgr(nm_service,
            nm_dbus_path, QDBusConnection::systemBus());
    auto reply = mgr.AddAndActivateConnection(connection, device, specific);
    reply.waitForFinished();
    if(!reply.isValid()) {
        qDebug() << "Creating hotspot failed: " << reply.error().message() << "\n";
    } else {
        qDebug() << "Successfully created wifi hotspot.\n";
    }
}

void disableHotspot(const QDBusObjectPath &path) {
    QDBusInterface device(nm_service, path.path(), nm_device_interface);
    QDBusReply<void> reply = device.call("Disconnect");
    if(!reply.isValid()) {
        qDebug() << "Disconnecting hotspot failed: " << reply.error().message() << "\n";
    }
}

#define NM_METHOD_NAME "AddAndActivateConnection"

int startAdhoc(const QByteArray &ssid, const QString &password, const QDBusObjectPath &devicePath) {
    nmConnectionArg connection;

    QDBusObjectPath specific("/");

    QVariantMap wireless;
    wireless[QString("security")] = QVariant(QString("802-11-wireless-security"));
    wireless[QString("ssid")] = QVariant(ssid);
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

    OrgFreedesktopNetworkManagerInterface mgr(nm_service,
            nm_dbus_path,
            QDBusConnection::systemBus());
    auto reply = mgr.AddAndActivateConnection(connection, devicePath, specific);
    reply.waitForFinished();
    if(!reply.isValid()) {
        printf("Failed to start adhoc network: %s\n", reply.error().message().toUtf8().data());
    }
    return 0;
}

#include<cassert>

bool detectAdhoc(QString &dbusPath, QByteArray &ssid, QString &password, bool &isActive) {
    static const QString activeIface("org.freedesktop.NetworkManager.Connection.Active");
    static const QString connProp("Connection");
    OrgFreedesktopNetworkManagerInterface mgr(nm_service,
            nm_dbus_path,
            QDBusConnection::systemBus());
    auto activeConnections = mgr.activeConnections();
    OrgFreedesktopNetworkManagerSettingsInterface settings(nm_service, nm_settings_object,
            QDBusConnection::systemBus());
    QSet<QDBusObjectPath> actives;
    auto r = settings.ListConnections();
    r.waitForFinished();
    for(const auto &conn : activeConnections) {
        QDBusInterface iface(nm_service, conn.path(), "org.freedesktop.DBus.Properties",
                QDBusConnection::systemBus());
        QDBusReply<QVariant> conname = iface.call("Get", activeIface, connProp);
        if(!conname.isValid()) {
            printf("Error: %s\n", conname.error().message().toUtf8().data());
            exit(1);
        }
        QDBusObjectPath mainConnection = qvariant_cast<QDBusObjectPath>(conname.value());
        //    printf("Conn: %s mainConn: %s\n",conn.path().toUtf8().data(), mainConnection.path().toUtf8().data());
        actives.insert(mainConnection);
    }
    const char wifiKey[] = "802-11-wireless";
    for(const auto &i : r.value()) {
        OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(nm_service,
                i.path(),
                QDBusConnection::systemBus());
        auto reply = conn.GetSettings();
        reply.waitForFinished();
        auto s = reply.value();
        if(s.find(wifiKey) != s.end()) {
            auto wsetup = s[wifiKey];
            if(wsetup["mode"] == "adhoc") {
                dbusPath = i.path();
                ssid = wsetup["ssid"].toByteArray();
                auto pwdReply = conn.GetSecrets("802-11-wireless-security");
                pwdReply.waitForFinished();
                password = pwdReply.value()["802-11-wireless-security"]["psk"].toString();
                isActive = false;
                for(const auto &ac : actives) {
                    if(i == ac) {
                        isActive = true;
                        break;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

QDBusObjectPath detectWirelessDevice() {
    OrgFreedesktopNetworkManagerInterface mgr(nm_service,
            nm_dbus_path,
            QDBusConnection::systemBus());
    auto devices = mgr.GetDevices();
    devices.waitForFinished();
    for(const auto &dpath : devices.value()) {
        QDBusInterface iface(nm_service, dpath.path(), "org.freedesktop.DBus.Properties",
                QDBusConnection::systemBus());
        QDBusReply<QVariant> typeReply = iface.call("Get", "org.freedesktop.NetworkManager.Device", "DeviceType");
        auto typeInt = qvariant_cast<int>(typeReply.value());
        if(typeInt == 2) {
            return dpath; // Assumptions are that there is only one wifi device and it is not hotpluggable.
        }
    }
    qWarning() << "Wireless device not found, hotspot functionality is inoperative.\n";
    return QDBusObjectPath();
}

}

CellularDbusHelper::CellularDbusHelper(QObject *parent) : QObject(parent),
        devicePath(detectWirelessDevice()) {
    static bool isRegistered = false;
    if(!isRegistered) {
        qDBusRegisterMetaType<nmConnectionArg>();
        isRegistered = true;
    }
    if(!detectAdhoc(settingsPath, ssid, password, isActive)) {
        settingsPath = "";
        ssid = "Ubuntu hotspot";
        password = "";
        isActive = false;
    }
}

QByteArray CellularDbusHelper::getHotspotName() {
    return ssid;
}

QString CellularDbusHelper::getHotspotPassword() {
    return password;
}

void CellularDbusHelper::setupHotspot(QByteArray ssid_, QString password_) {
    ssid = ssid_;
    password = password_;
    if(!settingsPath.isEmpty()) {
        // Prints a warning message if the connection has disappeared already.
        disableHotspot(); // Destroys all traces of the old setup, which is what we want.
    }
    startAdhoc(ssid, password, devicePath);
    detectAdhoc(settingsPath, ssid, password, isActive);
}

bool CellularDbusHelper::isHotspotActive() {
    return false;
}

void CellularDbusHelper::disableHotspot() {
    assert(!settingsPath.isEmpty());
    QDBusInterface control(nm_service, settingsPath, nm_connection_interface,
            QDBusConnection::systemBus());
    QDBusReply<void> reply = control.call("Delete");
    if(!reply.isValid()) {
        qWarning() << "Could not disconnect adhoc network: " << reply.error().message() << "\n";
    } else {
        isActive = false;
    }
}
