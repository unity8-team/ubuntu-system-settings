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
    m_lockSettings(QDir::home().filePath(".unity8-greeter-demo"),
                   QSettings::NativeFormat)
{
    m_lockSettings.beginGroup(qgetenv("USER"));

    connect (&m_accountsService,
             SIGNAL (propertyChanged (QString, QString)),
             this,
             SLOT (slotChanged (QString, QString)));

    connect (&m_accountsService,
             SIGNAL (nameOwnerChanged()),
             this,
             SLOT (slotNameOwnerChanged()));
}

void SecurityPrivacy::slotChanged(QString interface,
                                  QString property)
{
    if (interface != AS_INTERFACE)
        return;

    if (property == "MessagesWelcomeScreen") {
        Q_EMIT messagesWelcomeScreenChanged();
    } else if (property == "StatsWelcomeScreen") {
        Q_EMIT statsWelcomeScreenChanged();
    }
}

void SecurityPrivacy::slotNameOwnerChanged()
{
    // Tell QML so that it refreshes its view of the property
    Q_EMIT messagesWelcomeScreenChanged();
    Q_EMIT statsWelcomeScreenChanged();
}

bool SecurityPrivacy::getStatsWelcomeScreen()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "StatsWelcomeScreen").toBool();
}

void SecurityPrivacy::setStatsWelcomeScreen(bool enabled)
{
    if (enabled == getStatsWelcomeScreen())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "StatsWelcomeScreen",
                                      QVariant::fromValue(enabled));
    Q_EMIT(statsWelcomeScreenChanged());
}

bool SecurityPrivacy::getMessagesWelcomeScreen()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "MessagesWelcomeScreen").toBool();
}

void SecurityPrivacy::setMessagesWelcomeScreen(bool enabled)
{
    if (enabled == getMessagesWelcomeScreen())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "MessagesWelcomeScreen",
                                      QVariant::fromValue(enabled));
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

// XXX: passwordValue is invalid when security type is Swipe; handle this?
QString SecurityPrivacy::getSecurityValue()
{
    QVariant password(m_lockSettings.value("passwordValue", QString()));

    return password.toString();
}

void SecurityPrivacy::setSecurityValue(QString value)
{
    m_lockSettings.setValue("passwordValue", value);
    m_lockSettings.sync();
    Q_EMIT (securityValueChanged());
}
