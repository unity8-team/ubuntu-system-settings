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
#include <arpa/inet.h>

#include "nm_manager_proxy.h"
#include "nm_settings_proxy.h"
#include "nm_settings_connection_proxy.h"

#define NM_SERVICE "org.freedesktop.NetworkManager"
#define NM_PATH "/org/freedesktop/NetworkManager"
#define NM_DEVICE_IFACE "org.freedesktop.NetworkManager.Device"
#define NM_ACTIVE_CONNECTION_IFACE "org.freedesktop.NetworkManager.Connection.Active"

typedef QMap<QString,QVariantMap> ConfigurationData;
Q_DECLARE_METATYPE(ConfigurationData)


WifiDbusHelper::WifiDbusHelper(QObject *parent) : QObject(parent),
      m_systemBusConnection(QDBusConnection::systemBus())
{
    qDBusRegisterMetaType<ConfigurationData>();
}


QByteArray WifiDbusHelper::getCertContent(QString filename){
    QFile file(filename);
      if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not resolve Cert-File (" << filename << "): File does not exist or is empty." ;
            return QByteArray();
      }
      else {
            return file.readAll();
      }
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

    QByteArray cacert_a("file://");
    QByteArray clientcert("file://");
    QByteArray privatekey("file://");
    QByteArray pacFile;

    cacert_a.append( certs[0] );
    if (auth == 0) { // TLS
        wireless_802_1x["eap"] = QStringList("tls");
        wireless_802_1x["ca-cert"]  = cacert_a;

        clientcert.append( certs[1] );
        wireless_802_1x["client-cert"] = clientcert;

        privatekey.append( certs [2]);
        wireless_802_1x["private-key"] = privatekey;

        wireless_802_1x["private-key-password"] = password[0];
    } else if (auth == 1) { // TTLS
        wireless_802_1x["eap"] = QStringList("ttls");
        wireless_802_1x["ca-cert"]  = cacert_a;
        if (usernames[1] != "") {wireless_802_1x["anonymous-identity"]  = usernames[1];}
        if (!password[1].toInt()) {wireless_802_1x["password-flags"] = QString("2");}
    } else if (auth == 2) { // LEAP
        wireless_802_1x["eap"] = QStringList("leap");
    } else if (auth == 3) { // FAST
        wireless_802_1x["eap"] = QStringList("fast");
        wireless_802_1x["ca-cert"]  = cacert_a;
        if (usernames[1] != "") {wireless_802_1x["anonymous-identity"]  = usernames[1];}
        if (!password[1].toInt()) {wireless_802_1x["password-flags"] = QString("2");}

        if (certs[3].left(1) == "/"){
            pacFile = getCertContent(certs[3]);
        }
        else {
            pacFile.append(certs[3]);
        }
        wireless_802_1x["pac-file"]  = pacFile;
        wireless_802_1x["phase1-fast-provisioning"] = QString(certs[3].toInt()+1);
    } else if (auth == 4) { // PEAP
        wireless_802_1x["eap"] = QStringList("peap");
        wireless_802_1x["phase1-peaplabel"] = QString("1");
        if (usernames[1] != "") {wireless_802_1x["anonymous-identity"]  = usernames[1];}
        if (!password[1].toInt()) {wireless_802_1x["password-flags"] = QString("2");}
         //wireless_802_1x["phase1-peapver"] = QString("0"); #jkb:let us unset this until problems are reported.
    }

    if (auth == 1 || auth == 3 || auth == 4 ){ // only for TTLS, FAST and PEAP
        /* PAP      // index: 0
           MSCHAPv2 // index: 1
           MSCHAP   // index: 2
           CHAP     // index: 3
           GTC      // index: 4
           MD5      // index: 5        */
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
    for (const auto &d : devices) {
        QDBusInterface iface(NM_SERVICE,
                             d.path(),
                             NM_DEVICE_IFACE,
                             QDBusConnection::systemBus());

        auto type_v = iface.property("DeviceType");
        if (type_v.toUInt() == 2 /* NM_DEVICE_TYPE_WIFI */) {
            dev = d;
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
                                               QDBusObjectPath("/"),
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
    int ip4addr;

    QDBusObjectPath dev;
    for (const auto &d : devices) {
        QDBusInterface iface(NM_SERVICE, d.path(), NM_DEVICE_IFACE, m_systemBusConnection);
        auto type_v = iface.property("DeviceType");
        if (type_v.toUInt() == 2 /* NM_DEVICE_TYPE_WIFI */) {
            ip4addr = iface.property("Ip4Address").toInt();
            dev = d;
            break;
        }
    }

    if (dev.path().isEmpty()) {
        // didn't find a wifi device
        return QString();
    }

    if (!ip4addr) {
        // no ip address
        return QString();
    }


    struct in_addr ip_addr;
    ip_addr.s_addr = ip4addr;
    return QString(inet_ntoa(ip_addr));
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
