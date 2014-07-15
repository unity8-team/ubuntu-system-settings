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
#include <algorithm>

#include "nm_manager_proxy.h"
#include "nm_settings_proxy.h"
#include "nm_settings_connection_proxy.h"

typedef QMap<QString,QVariantMap> ConfigurationData;
Q_DECLARE_METATYPE(ConfigurationData)

WifiDbusHelper::WifiDbusHelper(QObject *parent) : QObject(parent)
{
    qDBusRegisterMetaType<ConfigurationData>();
}

void WifiDbusHelper::connect(QString ssid, int security, QString password)
{
    if(security<0 || security>2) {
        qWarning() << "Qml and C++ have gotten out of sync. Can't connect.\n";
        return;
    }

    OrgFreedesktopNetworkManagerInterface mgr("org.freedesktop.NetworkManager",
                                              "/org/freedesktop/NetworkManager",
                                              QDBusConnection::systemBus());

    QMap<QString, QVariantMap> configuration;

    QVariantMap connection;
    connection["type"] = QStringLiteral("802-11-wireless");
    configuration["connection"] = connection;

    QVariantMap wireless;
    wireless["ssid"] = ssid.toLatin1();

    // security:
    // 0: None
    // 1: WPA & WPA2 Personal
    // 2: WEP
    if (security != 0) {
        wireless["security"] = QStringLiteral("802-11-wireless-security");

        QVariantMap wireless_security;

        if (security == 1) {
            wireless_security["key-mgmt"] = QStringLiteral("wpa-psk");
            wireless_security["psk"] = password;
        } else if (security == 2) {
            wireless_security["key-mgmt"] = QStringLiteral("none");
            wireless_security["auth-alg"] = QStringLiteral("open");
            wireless_security["wep-key0"] = password;
            wireless_security["wep-key-type"] = QVariant(uint(1));
        }
        configuration["802-11-wireless-security"] = wireless_security;
    }

    configuration["802-11-wireless"] = wireless;


    // find the first wlan adapter for now
    auto reply1 = mgr.GetDevices();
    reply1.waitForFinished();
    if(!reply1.isValid()) {
        qWarning() << "Could not get network device: " << reply1.error().message() << "\n";
        return;
    }
    auto devices = reply1.value();

    QDBusObjectPath dev;
    for (const auto &d : devices) {
        QDBusInterface iface("org.freedesktop.NetworkManager",
                             d.path(),
                             "org.freedesktop.NetworkManager.Device",
                             QDBusConnection::systemBus());

        auto type_v = iface.property("DeviceType");
        if (type_v.toUInt() == 2 /* NM_DEVICE_TYPE_WIFI */) {
            dev = d;
            break;
        }
    }

    if (dev.path().isEmpty()) {
        // didn't find a wifi device
        return;
    }

    QDBusObjectPath tmp;
    auto reply2 = mgr.AddAndActivateConnection(configuration,
                                               dev,
                                               QDBusObjectPath("/"),
                                               tmp);
}

struct Network : public QObject
{
    struct DontCare : public std::exception {};

    QString id;

    enum class Mode {
        infrastructure,
        adhoc,
        unknown
    };
    Mode mode;

    enum class Type {
        wireless,
        other
    };
    Type type;

    qulonglong timestamp;

    bool isValid;
    QString path;

    enum class Security
    {
        open,
        secured
    };
    Security security;
    QString password;

    void parseConnection()
    {
        if (!settings.contains("connection"))
            throw DontCare();

        auto connection = settings["connection"];
        id = connection["id"].toString();

        // we only care about wifi
        auto type_str = connection["type"].toString();
        if (type_str != "802-11-wireless")
            throw DontCare();
        type = Type::wireless;

        auto match = connection.find("timestamp");
        if (match == connection.end()) {
            timestamp = 0;
        } else {
            timestamp = (*match).toULongLong();
        }
    }

    void parseWireless()
    {
        if (!settings.contains("802-11-wireless"))
            throw DontCare();

        auto wireless = settings["802-11-wireless"];

        auto mode_str = wireless["mode"];
        if (mode_str == "infrastructure")
            mode = Mode::infrastructure;
        else if (mode_str == "adhoc")
            mode = Mode::adhoc;
        else
            mode = Mode::unknown;

        auto match = wireless.find("security");
        if (match != wireless.end())
        {
            auto security_str = *match;
            if (security_str != "802-11-wireless-security")
                throw DontCare();
            security = Security::secured;
            parseWirelessSecurity();
        } else {
            security = Security::open;
        }
    }

    void parseWirelessSecurity()
    {
        if (!settings.contains("802-11-wireless-security"))
            return;

        auto security = settings["802-11-wireless-security"];
        auto keymgmt = security["key-mgmt"];
        auto authalg = security["auth-alg"];

        // If the connection has never been activated succesfully there is a
        // high chance that it has no stored secrects.
        if (timestamp != 0) {
            auto reply = m_iface.GetSecrets("802-11-wireless-security");
            reply.waitForFinished();
            if(!reply.isValid()) {
                qWarning() << "Error querying secrects: " << reply.error().message() << "\n";
                return;
            }
            auto secrects = reply.value();

            auto match = secrects.find("802-11-wireless-security");
            if (match != secrects.end()) {
                auto secrects_security = *match;

                if (keymgmt == "none") {
                    password = secrects_security["wep-key0"].toString();
                } else if (keymgmt == "wpa-psk" && authalg == "open") {
                    password = secrects_security["psk"].toString();
                } else {
                }
            }
        }
    }

    Network() = delete;
    Network(QString path)
        : path{path},
          m_iface("org.freedesktop.NetworkManager",
                  path,
                  QDBusConnection::systemBus())
    {
        auto reply = m_iface.GetSettings();
        reply.waitForFinished();
        if(!reply.isValid()) {
            qWarning() << "Error getting network info: " << reply.error().message() << "\n";
            throw DontCare();
        }
        settings = reply.value();

        try {
            parseConnection();
        } catch (const DontCare &) {
            qDebug() << "Ignoring a network based on connection block.\n";
            throw;
        }

        if (type == Type::wireless) {
            try {
                parseWireless();
            } catch (const DontCare &) {
                qDebug() << "Ignoring a network based on wireless block. " << qPrintable(m_iface.path()) << "\n";
                throw;
            }
        }
    }

    OrgFreedesktopNetworkManagerSettingsConnectionInterface m_iface;
    QMap<QString, QVariantMap> settings;
};

QList<QStringList> WifiDbusHelper::getPreviouslyConnectedWifiNetworks() {
    QList<QStringList> networks;

   OrgFreedesktopNetworkManagerSettingsInterface foo
            ("org.freedesktop.NetworkManager",
             "/org/freedesktop/NetworkManager/Settings",
             QDBusConnection::systemBus());
    auto reply = foo.ListConnections();
    reply.waitForFinished();
    if (reply.isValid()) {
        for(const auto &c: reply.value()) {
            try {
                Network network(c.path());
                QStringList tmp;
                tmp.push_back(network.id);
                tmp.push_back(network.path);
                tmp.push_back(network.password);
                QString lastConnected = "";
                QLocale locale;
                if (network.timestamp != 0) {
                    lastConnected = locale.toString(QDateTime::fromMSecsSinceEpoch(network.timestamp*1000), locale.dateFormat());
                }
                tmp.push_back(lastConnected);
                networks.push_back(tmp);
            } catch (const Network::DontCare &) {
                continue;
            }
        }
    } else {
        qWarning() << "ERROR " << reply.error().message() << "\n";
    }

    std::sort(networks.begin(), networks.end(), [](const QStringList &a, const QStringList &b){
        return a[0].toLower() < b[0].toLower();
    });
    return networks;
}

void WifiDbusHelper::forgetConnection(const QString dbus_path) {
    OrgFreedesktopNetworkManagerSettingsConnectionInterface bar
            ("org.freedesktop.NetworkManager",
             dbus_path,
             QDBusConnection::systemBus());
    auto reply = bar.Delete();
    reply.waitForFinished();
    if(!reply.isValid()) {
        qWarning() << "Error forgetting network: " << reply.error().message() << "\n";
    }
}
