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
 * Evan Dandrea <evan.dandrea@canonical.com>
 *
*/

#include "diagnostics.h"
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

Diagnostics::Diagnostics(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus())
{
    m_systemBusConnection.connect("com.ubuntu.WhoopsiePreferences",
                                  "/com/ubuntu/WhoopsiePreferences",
                                  "org.freedesktop.DBus.Properties",
                                  "PropertiesChanged",
                                  this,
                                  SLOT(slotChanged()));
    m_systemIdentifier = getIdentifier();
}

void Diagnostics::slotChanged()
{
    Q_EMIT reportCrashesChanged();
}

bool Diagnostics::canReportCrashes()
{
    QDBusInterface interface (
                "com.ubuntu.WhoopsiePreferences",
                "/com/ubuntu/WhoopsiePreferences",
                "com.ubuntu.WhoopsiePreferences",
                m_systemBusConnection,
                this);

    if (interface.isValid()) {
        return interface.property("ReportCrashes").toBool();
    }

    return false;
}

QString Diagnostics::getIdentifier()
{
    QDBusInterface interface (
                "com.ubuntu.WhoopsiePreferences",
                "/com/ubuntu/WhoopsiePreferences",
                "com.ubuntu.WhoopsiePreferences",
                m_systemBusConnection,
                this);
    QDBusReply<QString> reply = interface.call("GetIdentifier");

    if (reply.isValid()) {
        return reply.value();
    }
    return QString();
}

void Diagnostics::setReportCrashes(bool report)
{
    QDBusInterface interface (
                "com.ubuntu.WhoopsiePreferences",
                "/com/ubuntu/WhoopsiePreferences",
                "com.ubuntu.WhoopsiePreferences",
                m_systemBusConnection,
                this);

    if (interface.isValid()) {
        interface.call("SetReportCrashes", report);
    }
}

QString Diagnostics::systemIdentifier() {
    return m_systemIdentifier;
}

Diagnostics::~Diagnostics() {
}
