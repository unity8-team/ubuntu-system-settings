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
    m_watcher ("com.ubuntu.WhoopsiePreferences",
        QDBusConnection::systemBus(),
        QDBusServiceWatcher::WatchForOwnerChange),
    m_whoopsieInterface (
        "com.ubuntu.WhoopsiePreferences",
        "/com/ubuntu/WhoopsiePreferences",
        "com.ubuntu.WhoopsiePreferences",
        QDBusConnection::systemBus())
{
    connect(&m_watcher, SIGNAL(serviceOwnerChanged(QString, QString, QString)),
            this, SLOT(createInterface(QString, QString, QString)));
    createInterface("com.ubuntu.WhoopsiePreferences", "", "");
}

void Diagnostics::createInterface(const QString& name,
                                  const QString& oldOwner,
                                  const QString& newOwner)
{
    Q_UNUSED (oldOwner);

    if (!m_whoopsieInterface.connection().isConnected() ||
            name != "com.ubuntu.WhoopsiePreferences") {
        return;
    }

    m_whoopsieInterface.connection().connect(
        m_whoopsieInterface.service(),
        m_whoopsieInterface.path(),
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(slotChanged()));

    m_systemIdentifier = getIdentifier();

    if(!newOwner.isEmpty()) {
        /* Tell the UI to refresh its view of the properties */
        slotChanged();
    }
}

void Diagnostics::slotChanged()
{
    Q_EMIT reportCrashesChanged();
}

bool Diagnostics::canReportCrashes()
{
    if (m_whoopsieInterface.isValid()) {
        return m_whoopsieInterface.property("ReportCrashes").toBool();
    }
    return false;
}

QString Diagnostics::getIdentifier()
{
    QDBusReply<QString> reply = m_whoopsieInterface.call("GetIdentifier");
    if (reply.isValid()) {
        return reply.value();
    }
    return QString();
}

void Diagnostics::setReportCrashes(bool report)
{
    m_whoopsieInterface.call("SetReportCrashes", report);
    m_whoopsieInterface.call("SetAutomaticallyReportCrashes", report);
}

QString Diagnostics::systemIdentifier() {
    return m_systemIdentifier;
}

Diagnostics::~Diagnostics() {
}
