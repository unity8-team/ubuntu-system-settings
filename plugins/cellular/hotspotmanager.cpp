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
#include <QStringList>
#include <QDBusReply>
#include <QtDebug>
#include <QDBusInterface>
#include <QDBusMetaType>

typedef QMap<QString, QVariantMap> nmConnectionArg;
Q_DECLARE_METATYPE(nmConnectionArg)

namespace  { // wpa_supplicant interaction

const QString wpa_supplicant_service("fi.w1.wpa_supplicant1");
const QString wpa_supplicant_interface("fi.w1.wpa_supplicant1");
const QString wpa_supplicant_path("/fi/w1/wpa_supplicant1");

bool isHybrisWlan() {

  QString program("getprop");
  QStringList arguments;
  arguments << "urfkill.hybris.wlan";

  QProcess *getprop = new QProcess();
  getprop->start(program, arguments);

  if (!getprop->waitForFinished()) {
    qCritical() << "getprop process failed:" << getprop->errorString();
    delete getprop;
    return false;
  }

  int index = getprop->readAllStandardOutput().indexOf("1");
  delete getprop;

  // A non-negative integer means getprop returned 1
  return index >= 0;
}

// True if changed successfully, or there was no need. Otherwise false.
// Supported modes are 'p2p', 'sta' and 'ap'.
bool changeInterfaceFirmware(const QString interface, const QString mode) {

  // Not supported.
  if (mode == "adhoc") {
    return true;
  }

  if (isHybrisWlan()) {
    QDBusInterface wpasIface (
        wpa_supplicant_service, wpa_supplicant_path, wpa_supplicant_interface,
        QDBusConnection::systemBus());

    const QDBusObjectPath interface_path(interface);

    // TODO(jgdx): We need to guard against calling this
    // when the interface is not soft blocked.
    auto set_interface = wpasIface.call("SetInterfaceFirmware",
                                        QVariant::fromValue(interface_path),
                                        QVariant(mode));

    if (set_interface.type() == QDBusMessage::ErrorMessage) {
      qCritical() << "Failed to change interface firmware:"
          << set_interface.errorMessage();
      return false;
    } else {
      return true;
    }
  }

  // We had no need to change the firmware.
  return true;
}

} // wpa_supplicant interaction

namespace  { // UrfKill interaction

const QString urfkill_service("org.freedesktop.URfkill");
const QString urfkill_interface("org.freedesktop.URfkill");
const QString urfkill_path("/org/freedesktop/URfkill");

// True if call went through and returned true.
bool setWifiBlock(bool block) {

  QDBusInterface urfkill_dbus_interface(urfkill_service, urfkill_path,
                                        urfkill_interface, QDBusConnection::systemBus());

  const unsigned int device_type = 1; /* wifi type */
  auto reply = urfkill_dbus_interface.call("Block", device_type, block);

  if (reply.type() == QDBusMessage::ErrorMessage) {
    qCritical() << "Failed to block wifi" << reply.errorMessage();
    return false;
  }

  // We got exactly one argument back, and it's not an error.
  if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 1) {

    // returned false from call
    if (!qdbus_cast<bool>(reply.arguments().at(0))) {
      qCritical() << "URfkill Block call did not succeed";
      return false;
    } else {
      return true;
    }
  }
  // We did not get what we wanted from dbus, so we treat that as failure.
  return false;
}
} // UrfKill interaction

namespace { // NetworkManager interaction

const QString nm_service("org.freedesktop.NetworkManager");
const QString nm_object("/org/freedesktop/NetworkManager");
const QString nm_settings_object("/org/freedesktop/NetworkManager/Settings");
const QString nm_settings_interface("org.freedesktop.NetworkManager.Settings");
const QString nm_connection_interface("org.freedesktop.NetworkManager.Settings.Connection");
const QString nm_connection_active_interface("org.freedesktop.NetworkManager.Connection.Active");
const QString nm_device_interface("org.freedesktop.NetworkManager.Device");
const QString dbus_properties_interface("org.freedesktop.DBus.Properties");
const QString connection_property("Connection");

// NetworkManager dbus interface proxy we will use to query
// against NetworkManager. See
// https://developer.gnome.org/NetworkManager/0.9/spec.html
//     #org.freedesktop.NetworkManager
OrgFreedesktopNetworkManagerInterface nm_manager(
    nm_service, nm_object, QDBusConnection::systemBus());

// NetworkManager Settings interface proxy we use to get
// the list of connections, as well as adding connections.
// See https://developer.gnome.org/NetworkManager/0.9/spec.html
//     #org.freedesktop.NetworkManager.Settings
OrgFreedesktopNetworkManagerSettingsInterface nm_settings(
    nm_service, nm_settings_object, QDBusConnection::systemBus());

// Helper that maps QStrings to other QVariantMaps, i.e.
// QMap<QString, QVariantMap>. QVariantMap is an alias for
// QMap<QString, QVariant>.
// See http://doc.qt.io/qt-5/qvariant.html#QVariantMap-typedef and
// https://developer.gnome.org/NetworkManager/0.9/spec.html
//     #type-String_String_Variant_Map_Map
nmConnectionArg createConnectionArguments(
    const QByteArray &ssid, const QString &password,
    const QDBusObjectPath &devicePath, QString mode) {

  Q_UNUSED(devicePath);
  nmConnectionArg connection;

  QString s_ssid = QString::fromLatin1(ssid);

  QVariantMap wireless;
  wireless[QStringLiteral("security")] = QVariant(QStringLiteral("802-11-wireless-security"));
  wireless[QStringLiteral("ssid")] = QVariant(ssid);
  wireless[QStringLiteral("mode")] = QVariant(mode);

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

// Helper that returns a QMap<QString, QVariantMap> given a QDBusObjectPath.
// See https://developer.gnome.org/NetworkManager/0.9/spec.html
//     #org.freedesktop.NetworkManager.Settings.Connection.GetSettings
nmConnectionArg getConnectionSettings (QDBusObjectPath connection) {

  OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
      nm_service, connection.path(), QDBusConnection::systemBus());

  auto connection_settings = conn.GetSettings();
  connection_settings.waitForFinished();
  return connection_settings.value();
}


// Helper that returns a QMap<QString, QVariantMap> given a QDBusObjectPath.
// See https://developer.gnome.org/NetworkManager/0.9/spec.html
//     #org.freedesktop.NetworkManager.Settings.Connection.GetSettings
nmConnectionArg getConnectionSecrets (QDBusObjectPath connection,
                                      const QString key) {

  OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
      nm_service, connection.path(), QDBusConnection::systemBus());
  auto connection_secrets = conn.GetSecrets(key);
  connection_secrets.waitForFinished();
  return connection_secrets.value();
}

// Helper that adds a connection and returns the QDBusObjectPath
// of the newly created connection.
// See https://developer.gnome.org/NetworkManager/0.9/spec.html
//     #org.freedesktop.NetworkManager.Settings.AddConnection
QDBusObjectPath addConnection(
    const QByteArray &ssid, const QString &password,
    const QDBusObjectPath &devicePath, QString mode) {

  QDBusObjectPath invalid("");

  nmConnectionArg connection = createConnectionArguments(ssid, password,
                                                         devicePath, mode);

  auto add_connection_reply = nm_settings.AddConnection(connection);
  add_connection_reply.waitForFinished();

  if(!add_connection_reply.isValid()) {
    qCritical() << "Failed to add connection: "
        << add_connection_reply.error().message();
    return invalid;
  }
  return add_connection_reply.argumentAt<0>();
}

// Returns a QDBusObjectPath of a hotspot given a mode.
// Valid modes are 'p2p', 'ap' and 'adhoc'.
QDBusObjectPath getHotspot(QString mode) {
  const char wifi_key[] = "802-11-wireless";

  auto listed_connections = nm_settings.ListConnections();
  listed_connections.waitForFinished();

  for(const auto &connection : listed_connections.value()) {
    OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
        nm_service, connection.path(), QDBusConnection::systemBus());

    auto connection_settings = getConnectionSettings(connection);

    if(connection_settings.find(wifi_key) != connection_settings.end()) {
      auto wifi_setup = connection_settings[wifi_key];
      QString wifi_mode = wifi_setup["mode"].toString();

      if(wifi_mode == mode) {
        return connection;
      }
    }
  }
  return QDBusObjectPath();
}

// Returns a QDBusObjectPath of a wireless device. For now
// it returns the first device.
QDBusObjectPath getWirelessDevice () {

  // find the first wlan adapter for now
  auto reply1 = nm_manager.GetDevices();
  reply1.waitForFinished();

  if(!reply1.isValid()) {
    qCritical() << "Could not get network device: "
        << reply1.error().message();
    return QDBusObjectPath();
  }
  auto devices = reply1.value();

  QDBusObjectPath dev;
  for (const auto &d : devices) {
    QDBusInterface iface(nm_service, d.path(), nm_device_interface,
                         QDBusConnection::systemBus());
    auto type_v = iface.property("DeviceType");

    if (type_v.toUInt() == 2 /* NM_DEVICE_TYPE_WIFI */) {
      return d;
    }
  }
  qCritical() << "Wireless device not found, hotspot functionality is inoperative.";
  return dev;
}

// Helper to check if the hotspot on a given QDBusObjectPath is active
// or not. It checks if the Connection.Active [1] for the given
// path is in NetworkManager's ActiveConnections property [2].
// [1] https://developer.gnome.org/NetworkManager/0.9/spec.html
//     #org.freedesktop.NetworkManager.Connection.Active
// [2] https://developer.gnome.org/NetworkManager/0.9/spec.html
//     #org.freedesktop.NetworkManager
bool isHotspotActive (QDBusObjectPath hotspot_connection_path) {

  QSet<QDBusObjectPath> active_relevant_connections;
  auto active_connections = nm_manager.activeConnections();
  for(const auto &active_connection : active_connections) {

    QDBusInterface active_connection_dbus_interface(
        nm_service, active_connection.path(), dbus_properties_interface,
        QDBusConnection::systemBus());

    QDBusReply<QVariant> connection_property =
        active_connection_dbus_interface.call(
            "Get", nm_connection_active_interface, "Connection");

    if(!connection_property.isValid()) {
      qWarning() << "Failed to get Connection property from "
          << active_connection.path() << ": "
          << connection_property.error().message();
      continue;
    }
    QDBusObjectPath connection_path = qvariant_cast<QDBusObjectPath>(
        connection_property.value());

    if (hotspot_connection_path == connection_path) {
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

} // NetworkManager interaction

HotspotManager::HotspotManager(QObject *parent) :
      QObject(parent), m_mode("ap"), m_enabled(false), m_stored(false),
      m_password(generate_password().c_str()), m_ssid(QByteArray("Ubuntu")),
      m_device_path(getWirelessDevice()) {

  static bool isRegistered = false;

  if(!isRegistered) {
    qDBusRegisterMetaType<nmConnectionArg>();
    isRegistered = true;
  }

  // Stored is false if hotspot path is empty.
  m_hotspot_path = getHotspot(m_mode);
  setStored(!m_hotspot_path.path().isEmpty());

  if (m_stored) {
    updateSettingsFromDbus(m_hotspot_path);
  }
  Q_EMIT enabledChanged(m_enabled);

  // Watches for new connections added to NetworkManager's Settings
  // interface.
  nm_settings.connection().connect(
      nm_settings.service(), nm_settings_object, nm_settings_interface,
      "NewConnection", this, SLOT(onNewConnection(QDBusObjectPath)));

  // Watches changes in NetworkManager.
  nm_manager.connection().connect(
      nm_manager.service(), nm_object, nm_service, "PropertiesChanged", this,
      SLOT(onPropertiesChanged(QMap<QString, QVariant>)));
}

void HotspotManager::setEnabled(bool value) {

  bool blocked = setWifiBlock(true);

  if (!blocked) {
    // "The device could not be readied for configuration"
    Q_EMIT reportError(5);
    Q_EMIT enabledChanged(false);
    return;
  }

  // We are enabling a hotspot
  if (value) {

    bool changed = changeInterfaceFirmware("/", m_mode);
    qWarning() << "setEnabled, changeInterfaceFirmware" << changed;
    if (!changed) {
      // Necessary firmware for the device may be missing
      Q_EMIT reportError(35);
      Q_EMIT enabledChanged(false);
      setWifiBlock(false);
      return;
    }

    if (m_stored) {
      // we defer enabling until old hotspot is deleted
      // if we can delete the old one
      // If not, unset stored flag and call this method.
      if (!destroy(m_hotspot_path)) {
        setStored(false);
        setEnabled(true);
      }
    } else {
      // we defer enabling until new hotspot is created
      m_hotspot_path = addConnection(m_ssid, m_password, m_device_path, m_mode);
      if (m_hotspot_path.path().isEmpty()) {
        // Emit "Unknown Error".
        Q_EMIT reportError(0);
        Q_EMIT enabledChanged(false);
        setWifiBlock(false);
      }
    }

  } else {

    // Disabling the hotspot.
    disable();
    m_enabled = false;
    Q_EMIT enabledChanged(m_enabled);

    bool unblocked = setWifiBlock(false);
    if (!unblocked) {
      // "The device could not be readied for configuration"
      Q_EMIT reportError(5);
    }
  }
}

// Disables a hotspot.
void HotspotManager::disable() {

  QDBusObjectPath hotspot = getHotspot(m_mode);
  if (hotspot.path().isEmpty()) {
    qWarning() << "Could not find a hotspot setup to disable.\n";
    return;
  }

  auto active_connections = nm_manager.activeConnections();
  for(const auto &active_connection : active_connections) {

    QDBusInterface iface(
        nm_service, active_connection.path(),
        "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());

    QDBusReply<QVariant> conname = iface.call(
        "Get", nm_connection_active_interface, "Connection");

    QDBusObjectPath backingConnection = qvariant_cast<QDBusObjectPath>(
        conname.value());

    if(backingConnection == m_hotspot_path) {
      nm_manager.DeactivateConnection(active_connection);
      return;
    }
  }
  return;
}

bool HotspotManager::enabled() const {
  return m_enabled;
}

bool HotspotManager::stored() const {
  return m_stored;
}

void HotspotManager::setStored(bool value) {
  if (m_stored != value) {
    m_stored = value;
    Q_EMIT storedChanged(value);
  }
}

QByteArray HotspotManager::ssid() const {
  return m_ssid;
}

void HotspotManager::setSsid(QByteArray value) {
  if (m_ssid != value) {
    m_ssid = value;
    Q_EMIT ssidChanged(value);
  }
}

QString HotspotManager::password() const {
  return m_password;
}

void HotspotManager::setPassword(QString value) {
  if (m_password != value) {
    m_password = value;
    Q_EMIT passwordChanged(value);
  }
}

QString HotspotManager::mode() const {
  return m_mode;
}

void HotspotManager::setMode(QString value) {
  if (m_mode != value) {
    m_mode = value;
    Q_EMIT modeChanged(value);
  }
}

void HotspotManager::onNewConnection(QDBusObjectPath path) {

  if (path == m_hotspot_path) {

    // If a new hotspot was added, it is also given that
    // Wi-Fi has been soft blocked. We can now unblock Wi-Fi
    // and let NetworkManager pick up the newly created
    // connection.
    bool unblocked = setWifiBlock(false);
    if (!unblocked) {
      // "The device could not be readied for configuration"
      Q_EMIT reportError(5);
    }
  setStored(true);
  }
}


bool HotspotManager::destroy(QDBusObjectPath path) {

  if(path.path().isEmpty()) {
    return false;
  }

  OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
      nm_service, path.path(), QDBusConnection::systemBus());

  conn.connection().connect(conn.service(), path.path(), conn.interface(),
                            "Removed", this, SLOT(onRemoved()));

  auto del = conn.Delete();
  del.waitForFinished();
  return del.isValid();
}

void HotspotManager::onRemoved() {
  // The UI does not support direct deletion of a hotspot, and given how
  // hotspots currently work, every time we want to re-use a hotspot, we
  // delete it an add a new one.
  // Thus, if a hotspot was deleted, we now create a new one.
  m_hotspot_path = addConnection(m_ssid, m_password, m_device_path, m_mode);
}


void HotspotManager::onPropertiesChanged(QVariantMap properties) {

  // If we have no hotspot path, ignore changes in NetworkManager.
  if (m_hotspot_path.path().isEmpty()) {
    return;
  }

  // Set flag so we know that ActiveConnections changed.
  bool active_connection_changed = false;

  for(QVariantMap::const_iterator iter = properties.begin(); iter != properties.end(); ++iter) {

    if (iter.key() == "ActiveConnections") {

      active_connection_changed = true;

      const QDBusArgument args = qvariant_cast<QDBusArgument>(iter.value());
      if (args.currentType() == QDBusArgument::ArrayType) {
        args.beginArray();

        while (!args.atEnd()) {
          QDBusObjectPath path = qdbus_cast<QDBusObjectPath>(args);

          QDBusInterface active_connection_dbus_interface(
              nm_service, path.path(), dbus_properties_interface,
              QDBusConnection::systemBus());

          QDBusReply<QVariant> connection_property = active_connection_dbus_interface.call(
              "Get",
              nm_connection_active_interface,
              "Connection");

          if(!connection_property.isValid()) {
            qWarning() << "Error getting connection_property: "
                << connection_property.error().message() << "\n";
            continue;
          }

          QDBusObjectPath connection_path = qvariant_cast<QDBusObjectPath>(
              connection_property.value());

          // We see our connection as being active, so we emit that is
          // enabled and return.
          if (connection_path == m_hotspot_path) {
            qWarning() << "onPropertiesChanged: will set enabled to  true";
            if (!m_enabled) {
              m_enabled = true;
              Q_EMIT enabledChanged(m_enabled);
            }
            return;
          }
        }
        args.endArray();
      }
    }
  }

  // At this point ActiveConnections changed, but
  // our hotspot was not in that list.
  if (active_connection_changed) {
    if (m_enabled) {
      qWarning() << "onPropertiesChanged: will set enabled to false";
      m_enabled = false;
      Q_EMIT enabledChanged(m_enabled);
    }
  }
}

void HotspotManager::updateSettingsFromDbus(QDBusObjectPath path) {

  m_enabled = isHotspotActive(m_hotspot_path);

  nmConnectionArg settings = getConnectionSettings(path);
  const char wifi_key[] = "802-11-wireless";
  const char security_key[] = "802-11-wireless-security";

  if (settings.find(wifi_key) != settings.end()) {

    QByteArray ssid = settings[wifi_key]["ssid"].toByteArray();
    if (!ssid.isEmpty()) {
      setSsid(ssid);
    }

    QString mode = settings[wifi_key]["mode"].toString();
    if (!mode.isEmpty()) {
      setMode(mode);
    }
  }

  nmConnectionArg secrets = getConnectionSecrets(path, security_key);

  if (secrets.find(security_key) != secrets.end()) {

    QString pwd = secrets[security_key]["psk"].toString();
    if (!pwd.isEmpty()) {
      setPassword(pwd);
    }
  }
}
