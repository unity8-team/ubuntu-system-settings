/*
 * Copyright (C) 2013-2015 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Charles Kerr <charles.kerr@canonical.com>
 */

#ifndef USS_AETHERCAST_DEVICE_H
#define USS_AETHERCAST_DEVICE_H

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QSharedPointer>
#include <QString>

#include "freedesktop_properties.h"
#include "aethercast_device.h"
#include "dbus-shared.h"

struct Device: QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path
               READ getPath
               NOTIFY pathChanged)

    Q_PROPERTY(QString name
               READ getName
               NOTIFY nameChanged)

    Q_PROPERTY(QString address
               READ getAddress
               NOTIFY addressChanged)

    Q_PROPERTY(QString state
               READ getState
               NOTIFY stateChanged)

public:


Q_SIGNALS:
    void pathChanged();
    void nameChanged();
    void addressChanged();
    void stateChanged();
    void deviceChanged(); // catchall for any change

public:
    const QString& getName() const { return m_name; }
    const QString& getAddress() const { return m_address; }
    QString getState() const { return m_state; }
    QString getPath() const { return m_aethercastDevice ? m_aethercastDevice->path() : QString(); }

  private:
    QString m_name;
    QString m_state;
    QString m_address;
    QScopedPointer<AethercastDevice> m_aethercastDevice;
    QScopedPointer<FreeDesktopProperties> m_aethercastDeviceProperties;

  protected:
    void setName(const QString &name);
    void setAddress(const QString &address);
    void setState(QString state);

  public:
    Device() {}
    Device(const QString &path, QDBusConnection &bus);
    ~Device() {}
    void disconnect();
    void setProperties(const QMap<QString,QVariant> &properties);

  private Q_SLOTS:
    void slotPropertiesChanged(const QString &interface, const QVariantMap &changedProperties,
                               const QStringList &invalidatedProperties);

  private:
    void initDevice(const QString &path, QDBusConnection &bus);
    void updateProperties(QSharedPointer<QDBusInterface>);
    void updateProperty(const QString &key, const QVariant &value);
};

Q_DECLARE_METATYPE(Device*)

#endif // USS_AETHERCAST_DEVICE_H
