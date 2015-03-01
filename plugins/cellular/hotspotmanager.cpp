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

namespace {

const QString nm_service("org.freedesktop.NetworkManager");
const QString nm_settings_object("/org/freedesktop/NetworkManager/Settings");
const QString nm_settings_interface("org.freedesktop.NetworkManager.Settings");
const QString nm_connection_interface("org.freedesktop.NetworkManager.Settings.Connection");
const QString nm_dbus_path("/org/freedesktop/NetworkManager");
const QString nm_device_interface("org.freedesktop.NetworkManager.Device");

const QString wpas_service("fi.w1.wpa_supplicant1");
const QString wpas_interface("fi.w1.wpa_supplicant1");
const QString wpas_path("/fi/w1/wpa_supplicant1");

const QString urfkill_service("org.freedesktop.URfkill");
const QString urfkill_interface("org.freedesktop.URfkill");
const QString urfkill_path("/org/freedesktop/URfkill");

const QByteArray default_ap_name("Ubuntu Wi-Fi");

#define NM_METHOD_NAME "AddAndActivateConnection"

void startHotspot(const QByteArray &ssid, const QString &password, const QDBusObjectPath &devicePath, HotspotMode mode) {

    nmConnectionArg connection;

    QString s_ssid = QString::fromLatin1(ssid);

    QVariantMap wireless;
    wireless[QStringLiteral("security")] = QVariant(QStringLiteral("802-11-wireless-security"));
    wireless[QStringLiteral("ssid")] = QVariant(ssid);

    if (mode == HotspotMode::Adhoc) {
        wireless[QStringLiteral("mode")] = QVariant(QStringLiteral("adhoc"));
    } else if (mode == HotspotMode::Ap) {
        wireless[QStringLiteral("mode")] = QVariant(QStringLiteral("ap"));
    } else {
        qWarning() << "Not supported.";
        return;
    }

    connection["802-11-wireless"] = wireless;

    QVariantMap connsettings;
    connsettings[QStringLiteral("autoconnect")] = QVariant(false);


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

    QVariantMap security;
    security[QStringLiteral("proto")] = QVariant(QStringList{"rsn"});
    security[QStringLiteral("pairwise")] = QVariant(QStringList{"ccmp"});
    security[QStringLiteral("group")] = QVariant(QStringList{"ccmp"});
    security[QStringLiteral("key-mgmt")] = QVariant(QStringLiteral("wpa-psk"));
    security[QStringLiteral("psk")] = QVariant(password);
    connection["802-11-wireless-security"] = security;


    OrgFreedesktopNetworkManagerSettingsInterface settings(nm_service, nm_settings_object,
            QDBusConnection::systemBus());
    auto reply = settings.AddConnection(connection);
    reply.waitForFinished();
    if(!reply.isValid()) {
        qWarning() << "Failed to start hotspot: " << reply.error().message() << "\n";
    }

}


void startAdhoc(const QByteArray &ssid, const QString &password, const QDBusObjectPath &devicePath) {
    startHotspot(ssid, password, devicePath, HotspotMode::Adhoc);
}

void startAp(const QByteArray &ssid, const QString &password, const QDBusObjectPath &devicePath) {
    startHotspot(ssid, password, devicePath, HotspotMode::Ap);
}

bool detectHotspot(QString &dbusPath, QByteArray &ssid, QString &password, bool &isActive, HotspotMode mode) {
    qWarning() << "Detecting hotspot Step 0";
    static const QString activeIface("org.freedesktop.NetworkManager.Connection.Active");
    static const QString connProp("Connection");
    QByteArray hotspotMode("");


    if (mode == HotspotMode::Adhoc) {
        hotspotMode.clear();
        hotspotMode.append("adhoc");
    } else if (mode == HotspotMode::Ap) {
        hotspotMode.clear();
        hotspotMode.append("ap");
    } else {
        // not supported
        qWarning() << "Not supported.";
        return false;
    }
    qWarning() << "Detecting hotspot Step 1: mode" << hotspotMode;

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
            qWarning() << "Error getting conname: " << conname.error().message() << "\n";
            continue;
        }
        QDBusObjectPath mainConnection = qvariant_cast<QDBusObjectPath>(conname.value());
        // QDBusObjectPath mainConnection = QDBusObjectPath(conname.value().toString());
        qWarning() << "Adding to actives" << QDBusObjectPath(conname.value().toString()).path();
        actives.insert(mainConnection);
    }
    const char wifiKey[] = "802-11-wireless";
    qWarning() << "Detecting hotspot Step 2: activeConnection size" << actives.size(), r.value();

    for(const auto &i : r.value()) {
        qWarning() << "Detecting hotspot Step 2.1, checking" << i.path();
        OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(nm_service,
                i.path(),
                QDBusConnection::systemBus());
        auto reply = conn.GetSettings();
        reply.waitForFinished();
        auto s = reply.value();
        if(s.find(wifiKey) != s.end()) {
            qWarning() << "Detecting hotspot Step 3: found wifi key";
            auto wsetup = s[wifiKey];
            qWarning() << "Detecting hotspot Step 4: hotspotMode" << wsetup["mode"];
            if(wsetup["mode"] == hotspotMode) {
                qWarning() << "Detecting hotspot Step 5: found our hotspot";
                dbusPath = i.path();
                ssid = wsetup["ssid"].toByteArray();
                auto pwdReply = conn.GetSecrets("802-11-wireless-security");
                pwdReply.waitForFinished();
                password = pwdReply.value()["802-11-wireless-security"]["psk"].toString();
                isActive = false;
                for(const auto &ac : actives) {
                    qWarning() << "Checking" << ac.path() << "against" << i.path();
                    if(i == ac) {
                        isActive = true;
                        qWarning() << "Detecting hotspot Step 6: hotspot is also active";
                        break;
                    }
                }
                return true;
            }
        }
    }
    qWarning() << "Detecting hotspot Step 5: hotspot is not active";
    return false;

}

bool detectAdhoc(QString &dbusPath, QByteArray &ssid, QString &password, bool &isActive) {
    return detectHotspot(dbusPath, ssid, password, isActive, HotspotMode::Adhoc);
}

bool detectAp(QString &dbusPath, QByteArray &ssid, QString &password, bool &isActive) {
    return detectHotspot(dbusPath, ssid, password, isActive, HotspotMode::Ap);
}


QDBusObjectPath detectWirelessDevice() {
    OrgFreedesktopNetworkManagerInterface mgr(nm_service,
            nm_dbus_path,
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

std::string generate_password() {
    static const std::string items("abcdefghijklmnopqrstuvwxyz01234567890");
    const int passwordLength = 8;
    std::string result;
    for(int i=0; i<passwordLength; i++) {
        result.push_back(items[std::rand() % items.length()]);
    }
    return result;
}

}

HotspotManager::HotspotManager(QObject *parent) : QObject(parent),
        m_devicePath(detectWirelessDevice()) {
    static bool isRegistered = false;
    if(!isRegistered) {
        qDBusRegisterMetaType<nmConnectionArg>();
        isRegistered = true;
    }

    // Default mode is Ap.
    m_mode = HotspotMode::Ap;

    bool adhocActive = detectAdhoc(m_settingsPath, m_ssid, m_password, m_isActive);
    bool apActive = detectAp(m_settingsPath, m_ssid, m_password, m_isActive);

    if(!adhocActive && !apActive) {
        qWarning() << "HotspotManager: No hotspots active";
        m_settingsPath = "";
        m_ssid = default_ap_name;
        m_password = generate_password().c_str();
        m_isActive = false;
    }

    OrgFreedesktopNetworkManagerSettingsInterface settings(nm_service, nm_settings_object,
            QDBusConnection::systemBus());
    settings.connection().connect(
        settings.service(),
        nm_settings_object,
        nm_settings_interface,
        "NewConnection",
        this,
        SLOT(newConnection(QDBusObjectPath)));
}

void HotspotManager::newConnection(const QDBusObjectPath path) {
    qWarning() << "Saw new connection" << path.path();
    if (path.path() == m_settingsPath) {

        const QDBusObjectPath specific("/");
        const QDBusObjectPath settings(m_settingsPath);
        const QDBusObjectPath device(m_devicePath.path());

        qWarning() << path.path() << m_settingsPath;
        qWarning() << "enableHotspot: unblocking wifi...";
        setWifiBlock(false);
        OrgFreedesktopNetworkManagerInterface mgr(nm_service,
                nm_dbus_path,
                QDBusConnection::systemBus());
        // QDBusInterface connection(nm_service, m_settingsPath, nm_connection_interface,
        //         QDBusConnection::systemBus());
        OrgFreedesktopNetworkManagerSettingsConnectionInterface con(
            nm_service, m_settingsPath, QDBusConnection::systemBus());
        mgr.ActivateConnection(settings, device, specific);

    }
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

void HotspotManager::enableHotspot() {
    qWarning() << "enableHotspot: blocking wifi...";
    setWifiBlock(true);
    switch(m_mode) {
        case HotspotMode::Unknown:
            qWarning() << "Cannot provision unknown type hotspot.";
            break;
        case HotspotMode::Adhoc:
            qWarning() << "Enabling adhoc hotspot.";
            enableAdhocHotspot();
            break;
        case HotspotMode::Ap:
            qWarning() << "Enabling ap hotspot.";

            enableApHotspot();
            break;
        case HotspotMode::Infra:
            qWarning() << "Not implemented";
            break;
    }
}

void HotspotManager::enableAdhocHotspot() {
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

void HotspotManager::enableApHotspot() {
    if(!m_settingsPath.isEmpty()) {
        qWarning() << "enableApHotspot m_settingsPath" << m_settingsPath;
        // Prints a warning message if the connection has disappeared already.
        destroyHotspot();
        // NM returns from the dbus call immediately but only destroys the
        // connection some time later. There is no callback for when this happens.
        // So this is the best we can do with reasonable effort.
        QThread::sleep(1);
    }
    setupWpas();
    startAp(m_ssid, m_password, m_devicePath);
    detectAp(m_settingsPath, m_ssid, m_password, m_isActive);
}

bool HotspotManager::isHotspotActive() {
    qWarning() << "isHotspotActive" << m_isActive;
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
        // QDBusObjectPath backingConnection = QDBusObjectPath(conname.value().toString());
        if(backingConnection.path() == m_settingsPath) {
            qWarning() << "DeactivateConnection" << m_settingsPath;
            mgr.DeactivateConnection(aConn);
            return;
        }
    }
    qWarning() << "Could not find a hotspot setup to disable.\n";
}

void HotspotManager::destroyHotspot() {
    qWarning() << "destroyHotspot.\n";
    if(m_settingsPath.isEmpty()) {
        qWarning() << "Tried to destroy nonexisting hotspot.\n";
        return;
    }
    QDBusInterface control(nm_service, m_settingsPath, nm_connection_interface,
            QDBusConnection::systemBus());
    QDBusReply<void> reply = control.call("Delete");
    if(!reply.isValid()) {
        qWarning() << "Could not disconnect network: " << reply.error().message() << "\n";
    } else {
        m_isActive = false;
    }
}

void HotspotManager::setupWpas() {
    QString program("getprop");
    QStringList arguments;
    arguments << "urfkill.hybris.wlan";
    QProcess *getprop = new QProcess(this);
    getprop->start(program, arguments);

    if (!getprop->waitForFinished())
        qWarning() << "Failed to get prop:" << getprop->errorString();

    if (getprop->readAllStandardOutput().indexOf("1") >= 0) {
        qWarning() << "Had to poke wpa_supplicant.";
        QDBusInterface wpasIface (
            wpas_service,
            wpas_path,
            wpas_interface,
            QDBusConnection::systemBus(),
            this);

        auto reply = wpasIface.call("SetInterfaceFirmware", "/", "ap");
        if (reply.type() == QDBusMessage::ErrorMessage) {
            qCritical() << "Failed to poke wpa_supplicant" << reply.errorMessage();
        }
    } else {
        qWarning() << "No need to poke wpa_supplicant.";
    }

}

void HotspotManager::setWifiBlock(bool block) {

    QDBusInterface urfkillIface (
        urfkill_service,
        urfkill_path,
        urfkill_interface,
        QDBusConnection::systemBus(),
        this);


    const unsigned int idx = 1;
    auto reply = urfkillIface.call("Block", idx, block);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCritical() << "Failed to block wifi" << reply.errorMessage();
    }

    if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 1) {
        // returned false from call
        if (!qdbus_cast<bool>(reply.arguments().at(0))) {
           qCritical() << "URfkill Block call did not succeed";
        }
    }
}
