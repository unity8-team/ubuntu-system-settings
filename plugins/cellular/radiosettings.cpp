/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/

#include "radiosettings.h"
#include <QObject>
#include <QtDebug>
#include <QStringList>
#include <QDBusReply>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <glib.h>
#include <glib-object.h>
#include <unistd.h>
#include <iostream>

namespace {
    const QString c_service("org.ofono.RadioSettings");
    const QString c_object("/org/ofono/RadioSettings/");
}

RadioSettings::RadioSettings(QObject *parent) :
    QObject(parent),

    // connect to system bus
    m_systemBusConnection (QDBusConnection::systemBus()),

    // watch the service
    m_serviceWatcher (c_service,
                      m_systemBusConnection,
                      QDBusServiceWatcher::WatchForOwnerChange),
    // create an interface to radioSettigns
    m_radioSettingsInterface (c_service,
                         QString("/ril_0/%s").arg(c_object), // FIXME: make argument of modem
                         c_service,
                          m_systemBusConnection)
{
    // get notified when setting change
    connect (&m_serviceWatcher,
         SIGNAL (serviceOwnerChanged (QString, QString, QString)),
         this,
         SLOT (slotNameOwnerChanged (QString, QString, QString)));

    if (m_radioSettingsInterface.isValid()) {
        setUpInterface();
    }
}

void RadioSettings::setUpInterface()
{
    m_radioSettingsInterface.connection().connect(
        m_radioSettingsInterface.service(),
        m_radioSettingsInterface.path(),
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(slotChanged(QString, QVariantMap, QStringList)));
    }

void RadioSettings::slotChanged(QString interface,
                           QVariantMap changed_properties,
                           QStringList invalidated_properties)
{
    Q_UNUSED (interface);
    Q_UNUSED (changed_properties);
    Q_UNUSED (invalidated_properties);
    //QString slots(invalidated_properties.join(", "));
    printf("slotChanged %s.\n", "foo");
}

void RadioSettings::slotNameOwnerChanged(QString name,
                                    QString oldOwner,
                                    QString newOwner)
{
    Q_UNUSED (oldOwner);
    Q_UNUSED (newOwner);

    qDebug() << name << "foooo";

    if (name != c_service)
        return;

    setUpInterface();
    // Tell QML so that it refreshes its view of the property
    Q_EMIT technologyPreferenceChanged();
}

RadioSettings::~RadioSettings() {
}
