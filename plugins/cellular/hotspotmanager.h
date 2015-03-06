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

#ifndef CELLULAR_DBUS_HELPER
#define CELLULAR_DBUS_HELPER

#include <QObject>
#include <QDBusObjectPath>

#include "nm_manager_proxy.h"
#include "nm_settings_proxy.h"
#include "nm_settings_connection_proxy.h"

/**
 * For exposing dbus data to Qml.
 */

class HotspotManager : public QObject {
  Q_OBJECT
  Q_PROPERTY( bool enabled
    READ enabled
    WRITE setEnabled
    NOTIFY enabledChanged)
  Q_PROPERTY( QByteArray ssid
    READ ssid
    WRITE setSsid
    NOTIFY ssidChanged)
  Q_PROPERTY( QString securityScheme
    READ securityScheme
    WRITE setSecurityScheme
    NOTIFY securitySchemeChanged)
  Q_PROPERTY( QString password
    READ password
    WRITE setPassword
    NOTIFY passwordChanged)
  Q_PROPERTY( QString mode
    READ mode
    WRITE setMode
    NOTIFY modeChanged)
  Q_PROPERTY( bool stored
    READ stored
    NOTIFY storedChanged)

public:
  explicit HotspotManager(QObject *parent = nullptr);
  ~HotspotManager() {};

  bool enabled() const;
  void setEnabled(bool);

  bool stored() const;

  QByteArray ssid() const;
  void setSsid(QByteArray);

  QString password() const;
  void setPassword(QString);

  QString mode() const;
  void setMode(QString);

  QString securityScheme() const;
  void setSecurityScheme(QString);

  void destroyHotspot(QDBusObjectPath);

Q_SIGNALS:
//   void hotspotChanged(Hotspot hotspot);
  void enabledChanged(bool enabled);
  void storedChanged(bool stored);
  void pathChanged(QString path);
  void ssidChanged(QByteArray ssid);
  void passwordChanged(QString password);
  void securitySchemeChanged(QString securityScheme);
  void modeChanged(QString mode);
  void reportError(const QString &message);

public Q_SLOTS:
  void onCreateFinished(QDBusObjectPath);
  void onDeleteFinished();
  void onNetworkManagerPropertiesChanged(QVariantMap);
//   void hotspotEnabledChanged(bool);

private:
  QString m_mode;
  bool m_enabled;
  bool m_stored;
  QString m_password;
  QByteArray m_ssid;

  QDBusObjectPath m_device_path;
  QDBusObjectPath m_hotspot_path;

  bool enable();
  bool disable();
  void setStored(bool);

};

#endif
