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
 *
 * Authors:
 * Iain Lane <iain.lane@canonical.com>
 *
*/

#include "background.h"
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

Background::Background(QObject *parent) :
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

QString Background::getBackgroundFile()
{
    QDBusInterface userInterface (
                "org.freedesktop.Accounts",
                m_objectPath,
                "org.freedesktop.Accounts.User",
                m_systemBusConnection,
                this);

    if (userInterface.isValid()) {
        return userInterface.property("BackgroundFile").toString();
    }

    return QString();
}

void Background::slotChanged()
{
    QString new_background = getBackgroundFile();
    if (new_background != m_backgroundFile) {
        m_backgroundFile = new_background;
        Q_EMIT backgroundFileChanged();
    }
}

QString Background::backgroundFile()
{
    if (m_backgroundFile.isEmpty() || m_backgroundFile.isNull())
        m_backgroundFile = getBackgroundFile();

     return m_backgroundFile;
}

Background::~Background() {
}
