/*
 * Copyright (C) 2016 Canonical Ltd
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
 * Ken VanDine <ken.vandine@canonical.com>
 *
 */

#ifndef DISPLAYS_H
#define DISPLAYS_H

#include <QObject>
#include <QtDBus>
#include "devicemodel.h"
#include "aethercast_device.h"
#include "aethercast_manager.h"

class Displays : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QAbstractItemModel* devices
                READ devices
                NOTIFY devicesChanged)

    Q_PROPERTY (QAbstractItemModel* connectedDevices
                READ connectedDevices
                NOTIFY connectedDevicesChanged)

    Q_PROPERTY (QAbstractItemModel* disconnectedDevices
                READ disconnectedDevices
                NOTIFY disconnectedDevicesChanged)

    Q_PROPERTY (bool scanning
                READ scanning
                NOTIFY scanningChanged)

    Q_PROPERTY (QString state
                READ state
                NOTIFY stateChanged)

Q_SIGNALS:
    void devicesChanged();
    void connectedDevicesChanged();
    void disconnectedDevicesChanged();
    void scanningChanged(bool isActive);
    void stateChanged();

public:
    explicit Displays(QObject *parent = nullptr);
    explicit Displays(const QDBusConnection &dbus, QObject *parent = nullptr);
    ~Displays() {}

    Q_INVOKABLE void connectDevice(const QString &address);
    Q_INVOKABLE void disconnectDevice(const QString &address);
    Q_INVOKABLE void scan();
    void setProperties(const QMap<QString,QVariant> &properties);

public:
    QAbstractItemModel * devices();
    QAbstractItemModel * connectedDevices();
    QAbstractItemModel * disconnectedDevices();
    bool scanning() const { return m_manager->scanning(); }
    QString state() const { return m_manager->state(); }

private Q_SLOTS:
    void slotPropertiesChanged(const QString &interface, const QVariantMap &changedProperties,
                               const QStringList &invalidatedProperties);

private:
    QDBusConnection m_dbus;
    DeviceModel m_devices;
    DeviceFilter m_connectedDevices;
    DeviceFilter m_disconnectedDevices;
    AethercastManager* m_manager;
    QScopedPointer<FreeDesktopProperties> m_aethercastProperties;
    void updateProperties(QSharedPointer<QDBusInterface>);
    void updateProperty(const QString &key, const QVariant &value);
};

#endif // DISPLAYS_H
