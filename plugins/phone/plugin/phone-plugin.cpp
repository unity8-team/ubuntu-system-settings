/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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

#include "phone-plugin.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QProcessEnvironment>
#include <QtDBus>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

class PhoneItem: public ItemBase
{
    Q_OBJECT

public:
    explicit PhoneItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);
};


PhoneItem::PhoneItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains(QLatin1String("USS_SHOW_ALL_UI"))) {
        QString showAllS = env.value("USS_SHOW_ALL_UI", QString());

        if(!showAllS.isEmpty()) {
            setVisibility(true);
            return;
        }
    }

    bool supportedDevice(true);

    QDBusInterface m_NetStatusPropertiesIface(
            "com.ubuntu.connectivity1",
            "/com/ubuntu/connectivity1/NetworkingStatus",
            "org.freedesktop.DBus.Properties",
            QDBusConnection::sessionBus());
    QDBusPendingReply<QVariant> modemReply = m_NetStatusPropertiesIface.call(
        "Get", "com.ubuntu.connectivity1.NetworkingStatus", "ModemAvailable");
    modemReply.waitForFinished();
    if (modemReply.isValid()) {
        supportedDevice = modemReply.argumentAt<0>().toBool();
    }

    setVisibility(supportedDevice);
}

void PhoneItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *PhonePlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new PhoneItem(staticData, parent);
}

#include "phone-plugin.moc"
