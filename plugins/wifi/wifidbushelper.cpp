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
#include <QNetworkInterface>
#include <algorithm>
#include <arpa/inet.h>

#include "nm_manager_proxy.h"
#include "nm_settings_proxy.h"
#include "nm_settings_connection_proxy.h"

#define NM_SERVICE "org.freedesktop.NetworkManager"
#define NM_PATH "/org/freedesktop/NetworkManager"
#define NM_AP_IFACE "org.freedesktop.NetworkManager.AccessPoint"
#define NM_DEVICE_IFACE "org.freedesktop.NetworkManager.Device"
#define NM_DEVICE_WIRELESS_IFACE "org.freedesktop.NetworkManager.Device.Wireless"
#define NM_ACTIVE_CONNECTION_IFACE "org.freedesktop.NetworkManager.Connection.Active"

typedef QMap<QString,QVariantMap> ConfigurationData;
Q_DECLARE_METATYPE(ConfigurationData)

WifiDbusHelper::WifiDbusHelper(QObject *parent) : QObject(parent),
    m_systemBusConnection(QDBusConnection::systemBus())
{
    qDBusRegisterMetaType<ConfigurationData>();
}

void WifiDbusHelper::connect(QString ssid, int security, int auth, QStringList usernames, QStringList password, QStringList certs, int p2auth)
{
    if((security<0 || security>5) || (auth<0 || auth>4) || (p2auth<0 || p2auth>5)) {
        qWarning() << "Qml and C++ have gotten out of sync. Can't connect.\n";
        return;
    }

    OrgFreedesktopNetworkManagerInterface mgr(NM_SERVICE,
                                              NM_PATH,
                                              m_systemBusConnection);

    QMap<QString, QVariantMap> configuration;

    QVariantMap connection;
    connection["type"] = QStringLiteral("802-11-wireless");
    configuration["connection"] = connection;

    QVariantMap wireless;
    wireless["ssid"] = ssid.toLatin1();

    // security:
    // 0: None
    // 1: WPA & WPA2 Personal
    // 2: WPA Enterprise
    // 3: WEP
    // 4: Dynamic WEP
    // 5: LEAP
    if (security != 0) { // WPA Enterprise or Dynamic WEP
        wireless["security"] = QStringLiteral("802-11-wireless-security");

        QVariantMap wireless_security;

        if (security == 1) {
            wireless_security["key-mgmt"] = QStringLiteral("wpa-psk");
            wireless_security["psk"] = password[0];
        } else if (security == 3) {
            wireless_security["key-mgmt"] = QStringLiteral("none");
            wireless_security["auth-alg"] = QStringLiteral("open");
            wireless_security["wep-key0"] = password[0];
            wireless_security["wep-key-type"] = QVariant(uint(1));
        } else if (security == 2) {
            wireless_security["key-mgmt"] = QStringLiteral("wpa-eap");
        } else if (security == 4) {
            wireless_security["key-mgmt"] = QStringLiteral("ieee8021x");
            /* leave disabled as hopefully not needed:
            QStringList wep_pairwise, wep_group;
            wep_pairwise[0] ="wep40"; wep_pairwise[1] ="wep104";
            wep_group[0] ="wep40"; wep_group[1] ="wep104";
            wireless_security["pairwise"] = wep_pairwise;
            wireless_security["group"] = wep_group; */
        } else if (security == 5) {
            wireless_security["key-mgmt"] = QStringLiteral("ieee8021x");
            wireless_security["auth-alg"] = QStringLiteral("leap");
            wireless_security["leap-username"] = usernames[0];
            wireless_security["leap-password"] = password[0];
        }
        configuration["802-11-wireless-security"] = wireless_security;
    }

    configuration["802-11-wireless"] = wireless;

    if (security == 2 || security == 4){

        QVariantMap wireless_802_1x;
        // [802-1x]
        /*TLS   // index: 0
          TTLS  // index: 1
          LEAP  // index: 2
          FAST  // index: 3
          PEAP  // index: 4 */
        wireless_802_1x["identity"] = usernames[0];
        if (auth != 0) {
            wireless_802_1x["password"] = password[0];
        }

        QByteArray cacert(    "file://" + certs[0].toUtf8() + '\0');
        QByteArray clientcert("file://" + certs[1].toUtf8() + '\0');
        QByteArray privatekey("file://" + certs[2].toUtf8() + '\0');
        QString pacFile( certs[3] );

        if (auth == 0) { // TLS
            wireless_802_1x["eap"] = QStringList("tls");
            if (certs[0] != "") {wireless_802_1x["ca-cert"] = cacert;}
            if (certs[1] != "") {wireless_802_1x["client-cert"] = clientcert;}
            if (certs[2] != "") {wireless_802_1x["private-key"] = privatekey;}
            wireless_802_1x["private-key-password"] = password[0];
        } else if (auth == 1) { // TTLS
            wireless_802_1x["eap"] = QStringList("ttls");
            if (certs[0] != "") {wireless_802_1x["ca-cert"] = cacert;}
            if (usernames[1] != "") {wireless_802_1x["anonymous-identity"]  = usernames[1];}
            if (password[1] == "false") {wireless_802_1x["password-flags"]  = uint(2);}
        } else if (auth == 2) { // LEAP
            wireless_802_1x["eap"] = QStringList("leap");
        } else if (auth == 3) { // FAST
            wireless_802_1x["eap"] = QStringList("fast");
            if (certs[0] != "") {wireless_802_1x["ca-cert"] = cacert;}
            if (usernames[1] != "") {wireless_802_1x["anonymous-identity"]  = usernames[1];}
            if (password[1] == "false") {wireless_802_1x["password-flags"]  = uint(2);}
            if (certs[3] != "" ) {wireless_802_1x["pac-file"]  = pacFile;}
            wireless_802_1x["phase1-fast-provisioning"] = certs[4];
        } else if (auth == 4) { // PEAP
            wireless_802_1x["eap"] = QStringList("peap");
            if (certs[0] != "") {wireless_802_1x["ca-cert"] = cacert;}
            if (usernames[1] != "") {wireless_802_1x["anonymous-identity"]  = usernames[1];}
            if (password[1] == "false") {wireless_802_1x["password-flags"]  = uint(2);}
            if (certs[5] != "2") {wireless_802_1x["phase1-peapver"] = certs[5]; }
            // wireless_802_1x["phase1-peaplabel"] = QString("1"); #jkb:let us unset this until problems are reported.
        }

        if (auth == 1 || auth == 3 || auth == 4 ){ // only for TTLS, FAST and PEAP
            /* PAP      // index: 0
               MSCHAPv2 // index: 1
               MSCHAP   // index: 2
               CHAP     // index: 3
               GTC      // index: 4
               MD5      // index: 5 */
            if (p2auth == 0) {
                wireless_802_1x["phase2-auth"] = QStringLiteral("pap");
            } else if (p2auth == 1) {
                wireless_802_1x["phase2-auth"] = QStringLiteral("mschapv2");
            } else if (p2auth == 2) {
                wireless_802_1x["phase2-auth"] = QStringLiteral("mschap");
            } else if (p2auth == 3) {
                wireless_802_1x["phase2-auth"] = QStringLiteral("chap");
            } else if (p2auth == 4) {
                wireless_802_1x["phase2-auth"] = QStringLiteral("gtc");
            } else if (p2auth == 5) {
                wireless_802_1x["phase2-auth"] = QStringLiteral("md5");
            }
        }
        configuration["802-1x"] = wireless_802_1x;
    }

    // find the first wlan adapter for now
    auto reply1 = mgr.GetDevices();
    reply1.waitForFinished();
    if(!reply1.isValid()) {
        qWarning() << "Could not get network device: " << reply1.error().message() << "\n";
        return;
    }
    auto devices = reply1.value();

    QDBusObjectPath dev;
    QDBusObjectPath access_point("/");
    for (const auto &d : devices) {
        QDBusInterface iface(NM_SERVICE,
                             d.path(),
                             NM_DEVICE_IFACE,
                             QDBusConnection::systemBus());

        auto type_v = iface.property("DeviceType");
        if (type_v.toUInt() == 2 /* NM_DEVICE_TYPE_WIFI */) {

            // We found the device we want to connect.
            dev = d;

            // Create a proxy for Device.Wireless.
            QDBusInterface wiface(NM_SERVICE,
                                  d.path(),
                                  NM_DEVICE_WIRELESS_IFACE,
                                  QDBusConnection::systemBus());

            // Get a list of access points and use ssid to find the
            // one we're trying to connect to.
            QDBusMessage ap_msg = wiface.call("GetAllAccessPoints");
            if (ap_msg.type() == QDBusMessage::ReplyMessage && ap_msg.arguments().count() == 1) {

                // Get arguments.
                QList<QVariant> ap_variant = ap_msg.arguments();

                // Cast the first QVariant argument as QDBusArgument.
                QDBusArgument ap_argument = ap_variant.at(0).value<QDBusArgument>();

                // Cast the argument to a list of DBus object paths.
                QList<QDBusObjectPath> ap_list = qdbus_cast<QList<QDBusObjectPath>>(ap_argument);

                // Loop through the list of paths looking for our ssid.
                for(int i=0; i<ap_list.size(); ++i){

                    // Proxy for AccessPoint.
                    QDBusInterface aiface(NM_SERVICE,
                                          ap_list[i].path(),
                                          NM_AP_IFACE,
                                          QDBusConnection::systemBus());

                    auto ssid_v = aiface.property("Ssid");

                    if (QString::compare(ssid.toLatin1(), ssid_v.toString(), Qt::CaseSensitive) == 0) {
                        access_point = ap_list[i];
                        break;
                    }
                }
            }
            break;
        }
    }

    if (dev.path().isEmpty()) {
        // didn't find a wifi device
        qWarning() << "Could not find wifi device.";
        return;
    }

    mgr.connection().disconnect(
        mgr.service(),
        dev.path(),
        NM_DEVICE_IFACE,
        "StateChanged",
        this,
        SLOT(nmDeviceStateChanged(uint, uint, uint)));

    mgr.connection().connect(
        mgr.service(),
        dev.path(),
        NM_DEVICE_IFACE,
        "StateChanged",
        this,
        SLOT(nmDeviceStateChanged(uint, uint, uint)));

    QDBusObjectPath tmp;
    auto reply2 = mgr.AddAndActivateConnection(configuration,
                                               dev,
                                               access_point,
                                               tmp);
    if(!reply2.isValid()) {
        qWarning() << "Could not connect: " << reply2.error().message() << "\n";
    }
}


void WifiDbusHelper::nmDeviceStateChanged(uint newState,
                                         uint oldState,
                                         uint reason)
{
    Q_UNUSED (oldState);
    Q_EMIT (deviceStateChanged(newState, reason));
}

QString WifiDbusHelper::getWifiIpAddress()
{
    OrgFreedesktopNetworkManagerInterface mgr(NM_SERVICE,
                                              NM_PATH,
                                              m_systemBusConnection);

    // find the first wlan adapter for now
    auto reply1 = mgr.GetDevices();
    reply1.waitForFinished();
    if(!reply1.isValid()) {
        qWarning() << "Could not get network device: " << reply1.error().message() << "\n";
        return QString();
    }
    auto devices = reply1.value();

    for (const auto &d : devices) {
        QDBusInterface iface(NM_SERVICE, d.path(), NM_DEVICE_IFACE, m_systemBusConnection);
        auto type_v = iface.property("DeviceType");
        if (type_v.toUInt() == 2 /* NM_DEVICE_TYPE_WIFI */) {
            auto ip4name = iface.property("IpInterface").toString();

            QList<QHostAddress> adrs = QNetworkInterface::interfaceFromName(
                ip4name
            ).allAddresses();
            if (adrs.size() > 0) {
                return adrs.at(0).toString();
            }
            break;
        }
    }

    return QString();
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

            QString secretsType;
            if (keymgmt == "wpa-psk" && authalg == "open") {
                secretsType = "802-11-wireless-security";
            } else if (keymgmt == "wpa-eap" || keymgmt == "ieee8021x") {
                secretsType = "802-1x";
            }

            auto reply = m_iface.GetSecrets( secretsType );
            reply.waitForFinished();
            if(!reply.isValid()) {
                qWarning() << "Error querying secrects: " << reply.error().message() << "\n";
                return;
            }
            auto secrects = reply.value();
            auto match = secrects.find( secretsType );
            if (match != secrects.end()) {
                auto secrects_security = *match;

                if (keymgmt == "none") {
                    password = secrects_security["wep-key0"].toString();
                } else if (keymgmt == "wpa-psk" && authalg == "open") {
                    password = secrects_security["psk"].toString();
                } else if (keymgmt == "wpa-eap" || keymgmt == "ieee8021x") {
                    password = secrects_security["password"].toString();
                } else {
                }
            }
        }
    }

    Network() = delete;
    explicit Network(QString path)
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
            (NM_SERVICE,
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
            (NM_SERVICE,
             dbus_path,
             QDBusConnection::systemBus());
    auto reply = bar.Delete();
    reply.waitForFinished();
    if(!reply.isValid()) {
        qWarning() << "Error forgetting network: " << reply.error().message() << "\n";
    }
}

bool WifiDbusHelper::forgetActiveDevice() {
    OrgFreedesktopNetworkManagerInterface mgr(NM_SERVICE,
                                              NM_PATH,
                                              m_systemBusConnection);
    // find the first wlan adapter for now
    auto reply1 = mgr.GetDevices();
    reply1.waitForFinished();
    if(!reply1.isValid()) {
        qWarning() << __PRETTY_FUNCTION__ << ": Could not get network device: " << reply1.error().message() << "\n";
        return false;
    }
    auto devices = reply1.value();

    QDBusObjectPath dev;
    for (const auto &d : devices) {
        QDBusInterface iface(NM_SERVICE, d.path(), NM_DEVICE_IFACE, m_systemBusConnection);

        auto type_v = iface.property("DeviceType");
        if (type_v.toUInt() == 2 /* NM_DEVICE_TYPE_WIFI */) {
            if (d.path().isEmpty()) {
                // didn't find a wifi device
                qWarning() << __PRETTY_FUNCTION__ << ": Could not find wifi device\n";
                return false;
            } else {
                auto ac_path_var = iface.property("ActiveConnection");
                if(!ac_path_var.isValid()) {
                    qWarning() << __PRETTY_FUNCTION__ << ": Could not get active connection property from "
                               << d.path() << ".\n";
                    return true;
                }
                QString ac_path = ac_path_var.value<QDBusObjectPath>().path();
                QDBusInterface ac_iface(NM_SERVICE, ac_path, NM_ACTIVE_CONNECTION_IFACE, m_systemBusConnection);
                auto conn_path_var = ac_iface.property("Connection");
                if(!conn_path_var.isValid()) {
                    qWarning() << __PRETTY_FUNCTION__ << ": Could not get connection path property from "
                               << ac_path << ".\n";
                    return false;
                }
                forgetConnection(conn_path_var.value<QDBusObjectPath>().path());
            }
            break;
        }
    }
    return false;
}
