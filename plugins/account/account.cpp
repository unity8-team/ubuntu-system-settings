/*
 * Copyright (C) 2013 Canonical Ltd
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
*/

#include "account.h"
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

Account::Account(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_accountsserviceIface ("org.freedesktop.Accounts",
                            "/org/freedesktop/Accounts",
                            "org.freedesktop.Accounts",
                             m_systemBusConnection)
{
    if (!m_accountsserviceIface.isValid()) {
        return;
    }

    QDBusReply<QDBusObjectPath> qObjectPath = m_accountsserviceIface.call(
                "FindUserById", qlonglong(getuid()));

    if (qObjectPath.isValid()) {
        m_objectPath = qObjectPath.value().path();
    }

     m_systemBusConnection.connect("org.freedesktop.Accounts",
                                   m_objectPath,
                                   "org.freedesktop.Accounts.User",
                                   "Changed",
                                   this,
                                   SLOT(slotChanged()));
}

QString Account::getAccountRealName()
{
    QDBusInterface userInterface (
                "org.freedesktop.Accounts",
                m_objectPath,
                "org.freedesktop.Accounts.User",
                m_systemBusConnection,
                this);

    if (userInterface.isValid()) {
        return userInterface.property("RealName").toString();
    }

    return QString();
}

void Account::setAccountRealName(QString accountRealName)
{
    QDBusInterface userInterface (
                "org.freedesktop.Accounts",
                m_objectPath,
                "org.freedesktop.Accounts.User",
                m_systemBusConnection,
                this);

    if (!userInterface.isValid())
        return;

    userInterface.call("SetRealName", accountRealName);
}

void Account::slotChanged()
{
    QString new_account = getAccountRealName();
    if (new_account != m_accountRealName) {
        m_accountRealName = m_accountRealName;
        Q_EMIT accountRealNameChanged();
    }
}

QString Account::accountRealName()
{
    if (m_accountRealName.isEmpty() || m_accountRealName.isNull())
        m_accountRealName = getAccountRealName();

     return m_accountRealName;
}

Account::~Account() {
}
