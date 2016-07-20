/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FAKESYSTEMIMAGEDBUS_H
#define FAKESYSTEMIMAGEDBUS_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>

#include <libqtdbusmock/DBusMock.h>
#include <libqtdbustest/DBusTestRunner.h>

#define SI_SERVICE "com.canonical.SystemImage"
#define SI_IFACE "com.canonical.SystemImage"
#define SI_MAIN_OBJECT "/Service"

// #define FREEDESKTOP_PROPERTIES_IFACE "org.freedesktop.DBus.Properties"

using namespace QtDBusTest;
using namespace QtDBusMock;

class FakeSystemImageDbus : public QObject
{
    Q_OBJECT
private:
    DBusTestRunner m_dbusTestRunner;
    DBusMock m_dbusMock;
    QDBusInterface *m_siMock;
    QDBusInterface getInterface(const QString &path, const QString &interface);
public:
    explicit FakeSystemImageDbus(const QVariantMap &parameters, QObject *parent = 0);
    ~FakeSystemImageDbus();

    const QDBusConnection & dbus() { return m_dbusTestRunner.systemConnection(); }

Q_SIGNALS:
    void mockAvailableStatusChanged(const bool isAvailable,
                                    const bool downloading,
                                    const QString &availableVersion,
                                    const int &updateSize,
                                    const QString &lastUpdateDate,
                                    const QString &errorReason);
    void mockSettingChanged(const QString &key, const QString &value);
};

#endif // FAKESYSTEMIMAGEDBUS_H
