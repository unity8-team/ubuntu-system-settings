/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Michael Terry <michael.terry@canonical.com>
 *         Iain Lane <iain.lane@canonical.com>
 */

#include "securityprivacy.h"
#include <QtCore/QDir>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusVariant>
#include <unistd.h>
#include <sys/types.h>

#define AS_INTERFACE "com.ubuntu.touch.AccountsService.SecurityPrivacy"

SecurityPrivacy::SecurityPrivacy(QObject* parent)
  : QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_serviceWatcher ("org.freedesktop.Accounts",
                      m_systemBusConnection,
                      QDBusServiceWatcher::WatchForOwnerChange),
    m_accountsserviceIface ("org.freedesktop.Accounts",
                            "/org/freedesktop/Accounts",
                            "org.freedesktop.Accounts",
                             m_systemBusConnection),
    m_lockSettings(QDir::home().filePath(".unity8-greeter-demo"),
                   QSettings::NativeFormat)
{
    connect (&m_serviceWatcher,
             SIGNAL (serviceOwnerChanged (QString, QString, QString)),
             this,
             SLOT (slotNameOwnerChanged (QString, QString, QString)));

    if (m_accountsserviceIface.isValid()) {
        setUpInterface();
    }

}

void SecurityPrivacy::slotChanged(QString interface,
                                  QVariantMap changed_properties,
                                  QStringList invalidated_properties)
{
    Q_UNUSED (changed_properties);

    if (interface != AS_INTERFACE)
        return;

    if (invalidated_properties.contains("MessagesWelcomeScreen")) {
        Q_EMIT messagesWelcomeScreenChanged();
    }

    if (invalidated_properties.contains("StatsWelcomeScreen")) {
        Q_EMIT statsWelcomeScreenChanged();
    }
}

void SecurityPrivacy::setUpInterface()
{
    QDBusReply<QDBusObjectPath> qObjectPath = m_accountsserviceIface.call(
                "FindUserById", qlonglong(getuid()));

    if (qObjectPath.isValid()) {
        m_objectPath = qObjectPath.value().path();
        m_accountsserviceIface.connection().connect(
            m_accountsserviceIface.service(),
            m_objectPath,
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SLOT(slotChanged(QString, QVariantMap, QStringList)));
    }
}

void SecurityPrivacy::slotNameOwnerChanged(QString name,
                                           QString oldOwner,
                                           QString newOwner)
{
    Q_UNUSED (oldOwner);
    Q_UNUSED (newOwner);
    if (name != "org.freedesktop.Accounts")
        return;

    setUpInterface();
    // Tell QML so that it refreshes its view of the property
    Q_EMIT messagesWelcomeScreenChanged();
    Q_EMIT statsWelcomeScreenChanged();
}

QVariant SecurityPrivacy::getUserProperty(const QString &property)
{
    if (!m_accountsserviceIface.isValid())
        return QVariant();

    QDBusInterface iface (
                "org.freedesktop.Accounts",
                m_objectPath,
                "org.freedesktop.DBus.Properties",
                m_systemBusConnection,
                this);

    if (iface.isValid()) {
        QDBusReply<QDBusVariant> answer = iface.call(
                    "Get",
                    AS_INTERFACE,
                    property);
        if (answer.isValid()) {
            return answer.value().variant();
        }
    }
    return QVariant();
}

void SecurityPrivacy::setUserProperty(const QString &property,
                                      const QVariant &value)
{
    QDBusInterface iface (
                "org.freedesktop.Accounts",
                m_objectPath,
                "org.freedesktop.DBus.Properties",
                m_systemBusConnection,
                this);
    if (iface.isValid()) {
        // The value needs to be carefully wrapped
        iface.call("Set",
                   AS_INTERFACE,
                   property,
                   QVariant::fromValue(QDBusVariant(value)));
    }
}

bool SecurityPrivacy::getStatsWelcomeScreen()
{
    return getUserProperty("StatsWelcomeScreen").toBool();
}

void SecurityPrivacy::setStatsWelcomeScreen(bool enabled)
{
    if (enabled == getStatsWelcomeScreen())
        return;

    setUserProperty("StatsWelcomeScreen", QVariant::fromValue(enabled));
    Q_EMIT(statsWelcomeScreenChanged());
}

bool SecurityPrivacy::getMessagesWelcomeScreen()
{
    return getUserProperty("MessagesWelcomeScreen").toBool();
}

void SecurityPrivacy::setMessagesWelcomeScreen(bool enabled)
{
    if (enabled == getMessagesWelcomeScreen())
        return;

    setUserProperty("MessagesWelcomeScreen", QVariant::fromValue(enabled));
    Q_EMIT(messagesWelcomeScreenChanged());
}

SecurityPrivacy::SecurityType SecurityPrivacy::getSecurityType()
{
    QVariant password(m_lockSettings.value("password", "none"));

     if (password == "pin")
        return SecurityPrivacy::Passcode;
    else if (password == "keyboard")
        return SecurityPrivacy::Passphrase;
    else
         return SecurityPrivacy::Swipe;

}

void SecurityPrivacy::setSecurityType(SecurityType type)
{
    QVariant sec;

    switch (type) {
    case SecurityPrivacy::Passcode:
        sec = "pin";
        break;
    case SecurityPrivacy::Passphrase:
        sec = "keyboard";
        break;
    case SecurityPrivacy::Swipe:
    default:
        sec = "none";
        break;
    }

    m_lockSettings.setValue("password", sec);
    m_lockSettings.sync();
    Q_EMIT (securityTypeChanged());
}
