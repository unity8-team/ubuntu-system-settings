/*
 * Copyright (C) 2014, 2015 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *    Jonas G. Drange <jonas.drange@canonical.com>
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

#include "hotspotmanager.h"

#include "nm_manager_proxy.h"
#include "nm_settings_proxy.h"
#include "nm_settings_connection_proxy.h"
#include <QStringList>
#include <QDBusReply>
#include <QtDebug>
#include <QDBusInterface>
#include <QDBusMetaType>

typedef QMap<QString, QVariantMap> nmConnectionArg;
Q_DECLARE_METATYPE(nmConnectionArg)

/* wpa_supplicant interaction */
namespace  {

const QString wpa_supplicant_service("fi.w1.wpa_supplicant1");
const QString wpa_supplicant_interface("fi.w1.wpa_supplicant1");
const QString wpa_supplicant_path("/fi/w1/wpa_supplicant1");
const QString property("urfkill.hybris.wlan");

/* True if changed successfully, or there was no need. Otherwise false */
bool changeInterfaceFirmware(const QString interface, const QString mode) {

    QString program("getprop");
    QStringList arguments;
    arguments << property;
    QProcess *getprop = new QProcess();
    getprop->start(program, arguments);

    if (!getprop->waitForFinished()) {
        qWarning() << "Failed to get prop:" << getprop->errorString();
        return false;
    }

    if (getprop->readAllStandardOutput().indexOf("1") >= 0) {
        qWarning() << "Had to poke wpa_supplicant.";
        QDBusInterface wpasIface (wpa_supplicant_service,
            wpa_supplicant_path,
            wpa_supplicant_interface,
            QDBusConnection::systemBus());

        const QDBusObjectPath interface_path(interface);

        auto reply = wpasIface.call("SetInterfaceFirmware", QVariant::fromValue(interface_path), QVariant(mode));
        if (reply.type() == QDBusMessage::ErrorMessage) {
            qCritical() << "Failed to poke wpa_supplicant" << reply.errorMessage();
            return false;
        } else {
            qWarning() << "Poked wpa_supplicant, looking for new device";
            return true;
        }
    }

    qWarning() << "No need to poke wpa_supplicant.";
    return true;
}

} /* wpa_supplicant interaction */

/* UrfKill interaction */
namespace  {

const QString urfkill_service("org.freedesktop.URfkill");
const QString urfkill_interface("org.freedesktop.URfkill");
const QString urfkill_path("/org/freedesktop/URfkill");

/* True if call went through and returned true.*/
bool setWifiBlock(bool block) {

    QDBusInterface urfkill_dbus_interface (
        urfkill_service,
        urfkill_path,
        urfkill_interface,
        QDBusConnection::systemBus());

    const unsigned int device_type = 1; /* wifi type */
    auto reply = urfkill_dbus_interface.call("Block", device_type, block);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCritical() << "Failed to block wifi" << reply.errorMessage();
        return false;
    }

    if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 1) {
        // returned false from call
        if (!qdbus_cast<bool>(reply.arguments().at(0))) {
           qCritical() << "URfkill Block call did not succeed";
           return false;
        }
    }
    return true;
}

} /* UrfKill interaction */

/* NetworkManager interaction */
namespace {

const QString nm_service("org.freedesktop.NetworkManager");
const QString nm_object("/org/freedesktop/NetworkManager");
const QString nm_settings_object("/org/freedesktop/NetworkManager/Settings");
const QString nm_settings_interface("org.freedesktop.NetworkManager.Settings");
const QString nm_connection_interface("org.freedesktop.NetworkManager.Settings.Connection");
const QString nm_connection_active_interface("org.freedesktop.NetworkManager.Connection.Active");
const QString nm_device_interface("org.freedesktop.NetworkManager.Device");
const QString dbus_properties_interface("org.freedesktop.DBus.Properties");

nmConnectionArg getConnectionArg (const QByteArray &ssid, const QString &password, const QDBusObjectPath &devicePath, HotspotMode mode) {
    Q_UNUSED(devicePath);
    nmConnectionArg connection;

    QString s_ssid = QString::fromLatin1(ssid);

    QVariantMap wireless;
    wireless[QStringLiteral("security")] = QVariant(QStringLiteral("802-11-wireless-security"));
    wireless[QStringLiteral("ssid")] = QVariant(ssid);

    if (mode == HotspotMode::ADHOC) {
        wireless[QStringLiteral("mode")] = QVariant(QStringLiteral("adhoc"));
    } else if (mode == HotspotMode::AP) {
        wireless[QStringLiteral("mode")] = QVariant(QStringLiteral("ap"));
    }

    connection["802-11-wireless"] = wireless;

    QVariantMap connsettings;
    connsettings[QStringLiteral("autoconnect")] = QVariant(true);


    connsettings[QStringLiteral("id")] = QVariant(s_ssid);
    connsettings[QStringLiteral("uuid")] = QVariant(QStringLiteral("aab22b5d-7342-48dc-8920-1b7da31d6829"));
    connsettings[QStringLiteral("type")] = QVariant(QStringLiteral("802-11-wireless"));
    connection["connection"] = connsettings;

    QVariantMap ipv4;
    ipv4[QStringLiteral("addressess")] = QVariant(QStringList());
    ipv4[QStringLiteral("dns")] = QVariant(QStringList());
    ipv4[QStringLiteral("method")] = QVariant(QStringLiteral("shared"));
    ipv4[QStringLiteral("routes")] = QVariant(QStringList());
    connection["ipv4"] = ipv4;

    QVariantMap ipv6;
    ipv6[QStringLiteral("method")] = QVariant(QStringLiteral("ignore"));
    connection["ipv6"] = ipv6;

    QVariantMap security;
    security[QStringLiteral("proto")] = QVariant(QStringList{"rsn"});
    security[QStringLiteral("pairwise")] = QVariant(QStringList{"ccmp"});
    security[QStringLiteral("group")] = QVariant(QStringList{"ccmp"});
    security[QStringLiteral("key-mgmt")] = QVariant(QStringLiteral("wpa-psk"));
    security[QStringLiteral("psk")] = QVariant(password);
    connection["802-11-wireless-security"] = security;

    return connection;
}

QDBusObjectPath addHotspot(const QByteArray &ssid, const QString &password, const QDBusObjectPath &devicePath, HotspotMode mode) {

    QDBusObjectPath invalid("");

    switch(mode) {
        case HotspotMode::UNKNOWN:
            qWarning() << "Cannot provision unknown type hotspot.";
            return invalid;
        case HotspotMode::INFRA:
            qWarning() << "Not implemented";
            return invalid;
        default:
            break;
    }

    nmConnectionArg connection = getConnectionArg(ssid, password, devicePath, mode);

    OrgFreedesktopNetworkManagerSettingsInterface settings(nm_service, nm_settings_object,
            QDBusConnection::systemBus());

    auto reply = settings.AddConnection(connection);
    reply.waitForFinished();
    if(!reply.isValid()) {
        qWarning() << "Failed to add hotspot connection: " << reply.error().message() << "\n";
        return invalid;
    }

    return reply.argumentAt<0>();
}

QDBusObjectPath getHotspot(HotspotMode mode) {
    qWarning() << "getHotspot Step 0";

    QByteArray hotspotMode("");
    if (mode == HotspotMode::ADHOC) {
        hotspotMode = QByteArray("adhoc");
    } else if (mode == HotspotMode::AP) {
        hotspotMode = QByteArray("ap");
    } else {
        // not supported
        qWarning() << "Not supported.";
        return QDBusObjectPath();
    }

    qWarning() << "getHotspot Step 1: mode" << hotspotMode;

    const char wifi_key[] = "802-11-wireless";

    OrgFreedesktopNetworkManagerSettingsInterface settings(nm_service, nm_settings_object,
            QDBusConnection::systemBus());
    auto r = settings.ListConnections();
    r.waitForFinished();
    for(const auto &i : r.value()) {
        qWarning() << "getHotspot Step 2.1, checking" << i.path();
        OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(nm_service,
                i.path(),
                QDBusConnection::systemBus());
        auto reply = conn.GetSettings();
        reply.waitForFinished();
        auto s = reply.value();
        if(s.find(wifi_key) != s.end()) {
            qWarning() << "getHotspot Step 3: found wifi key";
            auto wifi_setup = s[wifi_key];
            qWarning() << "getHotspot Step 4: hotspotMode" << wifi_setup["mode"];
            if(wifi_setup["mode"] == hotspotMode) {
                qWarning() << "getHotspot Step 5: found a hotspot matching our mode" << hotspotMode;
                return i;
            }
        }
    }
    qWarning() << "getHotspot Step 5: found no hotspot";
    return QDBusObjectPath();
}

QDBusObjectPath getWirelessDevice () {
    OrgFreedesktopNetworkManagerInterface mgr(nm_service,
            nm_object,
            QDBusConnection::systemBus());
    // find the first wlan adapter for now
    auto reply1 = mgr.GetDevices();
    reply1.waitForFinished();
    if(!reply1.isValid()) {
        qWarning() << "Could not get network device: " << reply1.error().message() << "\n";
        return QDBusObjectPath();
    }
    auto devices = reply1.value();

    QDBusObjectPath dev;
    for (const auto &d : devices) {
        QDBusInterface iface(nm_service, d.path(),
            nm_device_interface,
            QDBusConnection::systemBus());
        auto type_v = iface.property("DeviceType");
        if (type_v.toUInt() == 2 /* NM_DEVICE_TYPE_WIFI */) {
            return d;
        }
    }

    qWarning() << "Wireless device not found, hotspot functionality is inoperative.\n";
    return dev;
}

bool isHotspotActive (QDBusObjectPath hotspot_connection_path) {
    static const QString connection_property("Connection");

    OrgFreedesktopNetworkManagerInterface manager_dbus_interface(nm_service,
        nm_object,
        QDBusConnection::systemBus());

    QSet<QDBusObjectPath> active_relevant_connections;
    auto active_connections = manager_dbus_interface.activeConnections();
    qWarning() << "isHotspotActive: searching" << active_connections.count() << "active connections.";
    for(const auto &active_connection : active_connections) {

        QDBusInterface active_connection_dbus_interface(nm_service,
            active_connection.path(),
            dbus_properties_interface,
            QDBusConnection::systemBus());

        QDBusReply<QVariant> connection_property = active_connection_dbus_interface.call("Get", nm_connection_active_interface, connection_property);
        if(!connection_property.isValid()) {
            qWarning() << "Error getting connection_property: " << connection_property.error().message() << "\n";
            continue;
        }
        QDBusObjectPath connection_path = qvariant_cast<QDBusObjectPath>(connection_property.value());
        qWarning() << "isHotspotActive: looking at" << connection_path.path() << "vs" << hotspot_connection_path.path();

        if (hotspot_connection_path == connection_path) {
            qWarning() << hotspot_connection_path.path() << "is active";
            return true;
        }

    }

    return false;
}

std::string generate_password() {
    static const std::string items("abcdefghijklmnopqrstuvwxyz01234567890");
    const int password_length = 8;
    std::string result;
    for(int i=0; i<password_length; i++) {
        result.push_back(items[std::rand() % items.length()]);
    }
    return result;
}

} /* NetworkManager interaction */

HotspotManager::HotspotManager(QObject *parent) : QObject(parent),
        m_device_path(getWirelessDevice()) {
    static bool is_registered = false;
    if(!is_registered) {
        qDBusRegisterMetaType<nmConnectionArg>();
        is_registered = true;
    }
    // Default mode is Ap.
    m_mode = HotspotMode::AP;

    QDBusObjectPath hotspot = getHotspot(m_mode);

    if(hotspot.path() == "")  {
        qWarning() << "HotspotManager: No hotspots found";
        m_active = false;
        m_settings_path = "";
        m_ssid = QByteArray("Ubuntu Wi-Fi");
        m_password = generate_password().c_str();
        qWarning() << "Generated pwd" << m_password;
    } else {
        qWarning() << "HotspotManager: Found a hotspot";
        m_settings_path = hotspot.path();
        OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
            nm_service, hotspot.path(),
            QDBusConnection::systemBus());

        auto password_reply = conn.GetSecrets("802-11-wireless-security");
        password_reply.waitForFinished();
        m_password = password_reply.value()["802-11-wireless-security"]["psk"].toString();

        auto connection_settings = conn.GetSettings();
        const char wifi_key[] = "802-11-wireless";
        connection_settings.waitForFinished();
        auto settings = connection_settings.value();
        auto wifi_setup = settings[wifi_key];
        m_ssid = wifi_setup["ssid"].toByteArray();
        m_active = isHotspotActive(hotspot);
    }
    Q_EMIT activeChanged(m_active);

    OrgFreedesktopNetworkManagerSettingsInterface settings(
        nm_service, nm_settings_object,
        QDBusConnection::systemBus());

    settings.connection().connect(
        settings.service(),
        nm_settings_object,
        nm_settings_interface,
        "NewConnection",
        this,
        SLOT(newConnection(QDBusObjectPath)));

    OrgFreedesktopNetworkManagerInterface manager_dbus_interface(nm_service,
        nm_object,
        QDBusConnection::systemBus());
}

void HotspotManager::newConnection(const QDBusObjectPath path) {
    qWarning() << "Saw new connection" << path.path() << "," << m_settings_path;
    if (path.path() == m_settings_path) {

        const QDBusObjectPath specific("/");
        const QDBusObjectPath settings(m_settings_path);
        const QDBusObjectPath device(m_device_path.path());

        qWarning() << "newConnection: poking wpa_supplicant...";

        switch(m_mode) {
            case HotspotMode::AP: {
                bool changed = changeInterfaceFirmware("/", "ap");
                if (!changed) {
                    Q_EMIT hotspotFailed("Failed to change firmware of device to ap.");
                    return;
                }
                break;
            }
            case HotspotMode::P2P: {
                bool changed = changeInterfaceFirmware("/", "p2p");
                if (!changed) {
                    Q_EMIT hotspotFailed("Failed to change firmware of device to p2p.");
                    return;
                }
                break;
            }
            default:
                qWarning() << "No need to change firmware.";
                break;
        }


        // OrgFreedesktopNetworkManagerInterface manager_dbus_interface(nm_service,
        //         nm_object,
        //         QDBusConnection::systemBus());
        // QDBusInterface connection(nm_service, m_settings_path, nm_connection_interface,
        //         QDBusConnection::systemBus());
        // OrgFreedesktopNetworkManagerSettingsConnectionInterface con(
        //     nm_service, m_settings_path, QDBusConnection::systemBus());

        //manager_dbus_interface.ActivateConnection(path, device, specific);
        //qWarning() << "ActivateConnection:" << path.path() << device.path() << specific.path();
        qWarning() << "newConnection: unblocking wifi...";
        bool unblocked = setWifiBlock(false);
        if (!unblocked) {
            qWarning() << "enableHotspot: failed to unblock wifi";
            Q_EMIT hotspotFailed("Failed to unblock wifi");
        }
    }
}


void HotspotManager::devicedAdded(const QDBusObjectPath path) {
    qWarning() << "Saw new device" << path.path();
}

QByteArray HotspotManager::getHotspotName() {
    return m_ssid;
}

QString HotspotManager::getHotspotPassword() {
    return m_password;
}

void HotspotManager::setupHotspot(QByteArray ssid_, QString password_, HotspotMode mode) {
    m_ssid = ssid_;
    m_password = password_;
    m_mode = mode;
}

bool HotspotManager::enableHotspot() {
    qWarning() << "enableHotspot...";

    bool blocked = setWifiBlock(true);

    if (!blocked) {
        qWarning() << "enableHotspot: failed to block wifi";
        Q_EMIT hotspotFailed("Failed to block wifi");
        return blocked;
    }

    QDBusObjectPath added = addHotspot(m_ssid, m_password, m_device_path, m_mode);

    if (added.path().isEmpty()) {
        qWarning() << "enableHotspot: failed to start hotspot";
        Q_EMIT hotspotFailed("Failed to start wifi");
        return false;
    }
    m_settings_path = added.path();

    bool unblocked = setWifiBlock(false);

    if (!unblocked) {
        qWarning() << "enableHotspot: failed to unblock wifi";
        Q_EMIT hotspotFailed("Failed to unblock wifi");
        return unblocked;
    }

    // all went well
    return true;
}

// void HotspotManager::enableAdhocHotspot() {
//     if(!m_settings_path.isEmpty()) {
//         // Prints a warning message if the connection has disappeared already.
//         destroyHotspot();
//         // NM returns from the dbus call immediately but only destroys the
//         // connection some time later. There is no callback for when this happens.
//         // So this is the best we can do with reasonable effort.
//         QThread::sleep(1);
//     }
//     startAdhoc(m_ssid, m_password, m_device_path);
//     detectAdhoc(m_settings_path, m_ssid, m_password);
// }

// void HotspotManager::enableApHotspot() {
//     if(!m_settings_path.isEmpty()) {
//         qWarning() << "enableApHotspot m_settings_path" << m_settings_path;
//         // Prints a warning message if the connection has disappeared already.
//         destroyHotspot();
//         // NM returns from the dbus call immediately but only destroys the
//         // connection some time later. There is no callback for when this happens.
//         // So this is the best we can do with reasonable effort.
//         QThread::sleep(1);
//     }
//     startAp(m_ssid, m_password, m_device_path);
//     detectAp(m_settings_path, m_ssid, m_password);
// }

void HotspotManager::setActive(bool value) {
    m_active = value;
    Q_EMIT activeChanged(value);

    if (value) {
        if (!enableHotspot()) {
            m_active = false;
            Q_EMIT activeChanged(false);
        }
    } else {
        if (!disableHotspot()) {
            m_active = true;
            Q_EMIT activeChanged(true);
        }
    }
}

bool HotspotManager::active() const {
    return m_active;
}

bool HotspotManager::disableHotspot() {
    static const QString activeIface("org.freedesktop.NetworkManager.Connection.Active");
    static const QString connProp("Connection");
    OrgFreedesktopNetworkManagerInterface manager_dbus_interface(nm_service,
            nm_object,
            QDBusConnection::systemBus());
    auto active_connections = manager_dbus_interface.activeConnections();
    for(const auto &active_connection : active_connections) {
        QDBusInterface iface(nm_service, active_connection.path(), "org.freedesktop.DBus.Properties",
                QDBusConnection::systemBus());
        QDBusReply<QVariant> conname = iface.call("Get", activeIface, connProp);
        QDBusObjectPath backingConnection = qvariant_cast<QDBusObjectPath>(conname.value());
        // QDBusObjectPath backingConnection = QDBusObjectPath(conname.value().toString());
        if(backingConnection.path() == m_settings_path) {
            qWarning() << "DeactivateConnection" << m_settings_path;
            manager_dbus_interface.DeactivateConnection(active_connection);
            m_active = false;
            return true;
        }
    }
    qWarning() << "Could not find a hotspot setup to disable.\n";
    return false;
}

void HotspotManager::destroyHotspot() {
    qWarning() << "destroyHotspot.\n";
    if(m_settings_path.isEmpty()) {
        qWarning() << "Tried to destroy nonexisting hotspot.\n";
        return;
    }
    QDBusInterface control(nm_service, m_settings_path, nm_connection_interface,
            QDBusConnection::systemBus());
    QDBusReply<void> reply = control.call("Delete");
    if(!reply.isValid()) {
        qWarning() << "Could not disconnect network: " << reply.error().message() << "\n";
    } else {
        m_active = false;
    }
}
