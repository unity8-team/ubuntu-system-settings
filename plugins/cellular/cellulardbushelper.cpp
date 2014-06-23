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
const QString settings_interface("org.freedesktop.NetworkManager.Settings");
const QString connection_interface("org.freedesktop.NetworkManager.Settings.Connection");
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

int start_adhoc() {
  nmConnectionArg connection;

  QDBusObjectPath device("/org/freedesktop/NetworkManager/Devices/0");
  QDBusObjectPath specific("/");

  QVariantMap wireless;
  wireless[QString("security")] = QVariant(QString("802-11-wireless-security"));
  wireless[QString("ssid")] = QVariant(QByteArray("mynetworkname"));
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
  security[QString("psk")] = QVariant(QString("qwerty1234"));
  connection["802-11-wireless-security"] = security;

  OrgFreedesktopNetworkManagerInterface mgr(nm_service,
                                            nm_dbus_path,
                                            QDBusConnection::systemBus());
  auto reply = mgr.AddAndActivateConnection(connection, device, specific);
  reply.waitForFinished();
  if(!reply.isValid()) {
    printf("Fail: %s\n", reply.error().message().toUtf8().data());
  } else {
    printf("Sukkess.\n");
  }

  return 0;
}

#include<cassert>

int detectAdhoc() {
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
        QByteArray ssid = wsetup["ssid"].toByteArray();
        auto pwdReply = conn.GetSecrets("802-11-wireless-security");
        pwdReply.waitForFinished();
        QString password = pwdReply.value()["802-11-wireless-security"]["psk"].toString();
        bool isActive = false;
        for(const auto &ac : actives) {
          printf("%s %s\n", ac.path().toUtf8().data(), i.path().toUtf8().data());
          if(i == ac) {
            isActive = true;
            break;
          }
        }
        printf("Is adhoc. Ssid: %s password: %s is_active: %d\n", ssid.data(), password.toUtf8().data(),
               isActive ? 1 : 0);
      }
    }
  }
  return 0;
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
