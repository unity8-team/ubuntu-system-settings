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
#include "hotspotmanager.h"
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

#define NM_METHOD_NAME "AddAndActivateConnection"

void startAdhoc(const QByteArray &ssid, const QString &password, const QDBusObjectPath &devicePath) {
    nmConnectionArg connection;

    QDBusObjectPath specific("/");

    QVariantMap wireless;
    wireless[QStringLiteral("security")] = QVariant(QStringLiteral("802-11-wireless-security"));
    wireless[QStringLiteral("ssid")] = QVariant(ssid);
    wireless[QStringLiteral("mode")] = QVariant(QStringLiteral("adhoc"));
    connection["802-11-wireless"] = wireless;

    QVariantMap connsettings;
    connsettings[QStringLiteral("autoconnect")] = QVariant(false);
    connsettings[QStringLiteral("uuid")] = QVariant(QStringLiteral("aab22b5d-7342-48dc-8920-1b7da31d6829"));
    connsettings[QStringLiteral("type")] = QVariant(QStringLiteral("802-11-wireless"));
    connection["connection"] = connsettings;

    QVariantMap ipv4;
    ipv4[QStringLiteral("addressess")] = QVariant(QStringList());
    ipv4[QStringLiteral("dns")] = QVariant(QStringList());
    ipv4[QStringLiteral("method")] = QVariant(QStringLiteral("shared"));
    ipv4[QStringLiteral("routes")] = QVariant(QStringList());
    connection["ipv4"] = ipv4;

    QVariantMap security;
    security[QStringLiteral("proto")] = QVariant(QStringList{"rsn"});
    security[QStringLiteral("pairwise")] = QVariant(QStringList{"ccmp"});
    security[QStringLiteral("group")] = QVariant(QStringList{"ccmp"});
    security[QStringLiteral("key-mgmt")] = QVariant(QStringLiteral("wpa-psk"));
    security[QStringLiteral("psk")] = QVariant(password);
    connection["802-11-wireless-security"] = security;

    OrgFreedesktopNetworkManagerInterface mgr(nm_service,
            nm_dbus_path,
            QDBusConnection::systemBus());
    auto reply = mgr.AddAndActivateConnection(connection, devicePath, specific);
    reply.waitForFinished();
    if(!reply.isValid()) {
        qWarning() << "Failed to start adhoc network: " << reply.error().message() << "\n";
    }
}

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
            qWarning() << "Error getting connamd: " << conname.error().message() << "\n";
            continue;
        }
        QDBusObjectPath mainConnection = qvariant_cast<QDBusObjectPath>(conname.value());
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

HotspotManager::HotspotManager(QObject *parent) : QObject(parent),
        m_devicePath(detectWirelessDevice()) {
    static bool isRegistered = false;
    if(!isRegistered) {
        qDBusRegisterMetaType<nmConnectionArg>();
        isRegistered = true;
    }
    if(!detectAdhoc(m_settingsPath, m_ssid, m_password, m_isActive)) {
        m_settingsPath = "";
        m_ssid = "Ubuntu hotspot";
        m_password = "";
        m_isActive = false;
    }
}

QByteArray HotspotManager::getHotspotName() {
    return m_ssid;
}

QString HotspotManager::getHotspotPassword() {
    return m_password;
}

void HotspotManager::setupHotspot(QByteArray ssid_, QString password_) {
    m_ssid = ssid_;
    m_password = password_;
    if(!m_settingsPath.isEmpty()) {
        // Prints a warning message if the connection has disappeared already.
        destroyHotspot();
        // NM returns from the dbus call immediately but only destroys the
        // connection some time later. There is no callback for when this happens.
        // So this is the best we can do with reasonable effort.
        QThread::sleep(1);
    }
    startAdhoc(m_ssid, m_password, m_devicePath);
    detectAdhoc(m_settingsPath, m_ssid, m_password, m_isActive);
}

bool HotspotManager::isHotspotActive() {
    return m_isActive;
}

void HotspotManager::disableHotspot() {
    static const QString activeIface("org.freedesktop.NetworkManager.Connection.Active");
    static const QString connProp("Connection");
    OrgFreedesktopNetworkManagerInterface mgr(nm_service,
            nm_dbus_path,
            QDBusConnection::systemBus());
    auto activeConnections = mgr.activeConnections();
    for(const auto &aConn : activeConnections) {
        QDBusInterface iface(nm_service, aConn.path(), "org.freedesktop.DBus.Properties",
                QDBusConnection::systemBus());
        QDBusReply<QVariant> conname = iface.call("Get", activeIface, connProp);
        QDBusObjectPath backingConnection = qvariant_cast<QDBusObjectPath>(conname.value());
        if(backingConnection.path() == m_settingsPath) {
            mgr.DeactivateConnection(aConn);
            return;
        }
    }
    qWarning() << "Could not find a hotspot setup to disable.\n";
}

void HotspotManager::destroyHotspot() {
    if(m_settingsPath.isEmpty()) {
        qWarning() << "Tried to destroy nonexisting hotspot.\n";
        return;
    }
    QDBusInterface control(nm_service, m_settingsPath, nm_connection_interface,
            QDBusConnection::systemBus());
    QDBusReply<void> reply = control.call("Delete");
    if(!reply.isValid()) {
        qWarning() << "Could not disconnect adhoc network: " << reply.error().message() << "\n";
    } else {
        m_isActive = false;
    }
}
