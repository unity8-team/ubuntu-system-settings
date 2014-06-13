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

#include "nm_settings_proxy.h"
#include "nm_settings_connection_proxy.h"

#include <iostream>

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

struct Network : public QObject
{
    struct DontCare : public std::exception {};

    QString id;

    enum class Mode {
        infrastructure,
        adhoc
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

        // if the configuration does not have timestamp it means
        // it has never been succesfully activated and there is no
        // point in showing it.
        if (!connection.contains("timestamp"))
            throw DontCare();
        timestamp = connection["timestamp"].toULongLong();
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
            throw DontCare();

        if (wireless.contains("security"))
        {
            auto security_str = wireless["security"];
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
            throw DontCare();

        auto security = settings["802-11-wireless-security"];
        auto keymgmt = security["key-mgmt"];
        auto authalg = security["auth-alg"];

        auto reply = m_iface.GetSecrets("802-11-wireless-security");
        reply.waitForFinished();
        auto secrects = reply.value();

        if (!secrects.contains("802-11-wireless-security"))
            throw DontCare();

        auto secrects_security = secrects["802-11-wireless-security"];

        if (keymgmt == "none") {
            if (!secrects_security.contains("wep-key0"))
                throw DontCare();
            password = secrects_security["wep-key0"].toString();
        } else if (keymgmt == "wpa-psk" && authalg == "open") {
            if (!secrects_security.contains("psk"))
                throw DontCare();
            password = secrects_security["psk"].toString();
        } else {
            throw DontCare();
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
        settings = reply.value();

        parseConnection();
        if (type == Type::wireless)
            parseWireless();
    }

    OrgFreedesktopNetworkManagerSettingsConnectionInterface m_iface;
    QMap<QString, QVariantMap> settings;
};

QList<QStringList> WifiDbusHelper::getPreviouslyConnectedWifiNetworks() {
    QList<QStringList> networks;

   OrgFreedesktopNetworkManagerSettingsInterface foo
            ("org.freedesktop.NetworkManager",
             "/org/freedesktop/NetworkManager/Settings",
             QDBusConnection::systemBus(),
             this);
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
                QLocale locale;
                tmp.push_back(locale.toString(QDateTime::fromMSecsSinceEpoch(network.timestamp*1000), locale.dateFormat()));
                networks.push_back(tmp);
            } catch (const Network::DontCare &) {
                continue;
            }
        }
    } else {
        std::cout << "ERROR" << qPrintable(reply.error().message()) << std::endl;
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
             QDBusConnection::systemBus(),
             this);
    auto reply = bar.Delete();
    reply.waitForFinished();
    printf("Forgotten network with path %s.\n", dbus_path.toUtf8().data());
}
