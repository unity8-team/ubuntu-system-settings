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

RadioSettings::RadioSettings(QObject *parent) :
    QObject(parent),

    // connect to system bus
    m_systemBusConnection (QDBusConnection::systemBus()),

    // watch the service
    m_serviceWatcher ("org.ofono.RadioSettings",
                      m_systemBusConnection,
                      QDBusServiceWatcher::WatchForOwnerChange),
    m_radioSettingsInterface ("org.ofono.RadioSettings",
                         "/org/ofono/RadioSettings/ril_0/", // FIXME: less hard coding of modem
                         "org.ofono.RadioSettings",
                          m_systemBusConnection)
{
    // get notified when setting change
    connect (&m_serviceWatcher,
         SIGNAL (serviceOwnerChanged (QString, QString, QString)),
         this,
         SLOT (slotNameOwnerChanged (QString, QString, QString)));

    if (m_radioSettingsInterface.isValid()) {
        qCritical() << "RadioSettings: valid interface, setting up interface";
        setUpInterface();
    } else {
        qCritical() << "RadioSettings: failed to set up interface";
    }

    qCritical() << "RadioSettings: ctor";
    QString technologyPreference("any");

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
        qCritical() << "radiosettings: setUpInterface";
    }

void RadioSettings::slotChanged(QString interface,
                           QVariantMap changed_properties,
                           QStringList invalidated_properties)
{
    Q_UNUSED (interface);
    Q_UNUSED (changed_properties);
    Q_UNUSED (invalidated_properties);
    // TODO
    qCritical() << "radiosettings: slotChanged";
}

// FIXME: figure out what exactly is required here
void RadioSettings::slotNameOwnerChanged(QString name,
                                    QString oldOwner,
                                    QString newOwner)
{
    Q_UNUSED (oldOwner);
    Q_UNUSED (newOwner);

    if (name != QString("org.ofono.RadioSettings"))
        return;

    setUpInterface();
}

// destructor
RadioSettings::~RadioSettings() {
}
