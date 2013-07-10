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

#include "diagnostics.h"
#include <QEvent>
#include <QDBusReply>
#include <iostream>
#include <unistd.h>

Diagnostics::Diagnostics(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus())
{
    std::cout << "created" << std::endl;
    m_systemBusConnection.connect("com.ubuntu.WhoopsiePreferences",
                                  "/com/ubuntu/WhoopsiePreferences",
                                  "org.freedesktop.DBus.Properties",
                                  "PropertiesChanged",
                                  this,
                                  SLOT(slotChanged()));
}

void Diagnostics::slotChanged()
{
    std::cout << "reportCrashesChanged() from c++" << std::endl;
    Q_EMIT reportCrashesChanged();
}

bool Diagnostics::canReportCrashes()
{
    std::cout << "canReportCrashes() from c++" << std::endl;
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

Diagnostics::~Diagnostics() {
}
