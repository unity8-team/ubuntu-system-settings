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

namespace
{
namespace whoopsie
{
constexpr const char* service_name{"com.ubuntu.WhoopsiePreferences"};
constexpr const char* object_path{"/com/ubuntu/WhoopsiePreferences"};
constexpr const char* interface_name{"com.ubuntu.WhoopsiePreferences"};
}

namespace location_service
{
constexpr const char* service_name{"com.ubuntu.location.Service"};
constexpr const char* object_path{"/com/ubuntu/location/Service"};
constexpr const char* interface_name{"com.ubuntu.location.Service"};
}
}

Diagnostics::Diagnostics(QObject *parent) :
    QObject(parent),
    m_watcher (),
    m_whoopsieInterface (
        whoopsie::service_name,
        whoopsie::object_path,
        whoopsie::interface_name,
        QDBusConnection::systemBus()),
    m_locationServiceInterface(
        location_service::service_name,
        location_service::object_path,
        location_service::interface_name,
        QDBusConnection::systemBus())
{
    m_watcher.setConnection(QDBusConnection::systemBus());
    m_watcher.setWatchMode(QDBusServiceWatcher::WatchForOwnerChange);
    m_watcher.addWatchedService(whoopsie::service_name);
    m_watcher.addWatchedService(location_service::service_name);

    connect(&m_watcher,
            SIGNAL(serviceOwnerChanged(QString, QString, QString)),
            this,
            SLOT(createWhoopsieInterface(QString, QString, QString)));

    connect(&m_watcher,
            SIGNAL(serviceOwnerChanged(QString, QString, QString)),
            this,
            SLOT(createLocationServiceInterface(QString, QString, QString)));

    createWhoopsieInterface(whoopsie::service_name, "", "");
    createLocationServiceInterface(location_service::service_name, "", "");
}

void Diagnostics::createWhoopsieInterface(const QString& name,
                                          const QString& oldOwner,
                                          const QString& newOwner)
{
    Q_UNUSED (oldOwner);

    if (!m_whoopsieInterface.connection().isConnected() ||
            name != whoopsie::service_name) {
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

void Diagnostics::createLocationServiceInterface(const QString& name,
                                                 const QString& oldOwner,
                                                 const QString& newOwner)
{
    Q_UNUSED (oldOwner);

    if (!m_locationServiceInterface.connection().isConnected() ||
            name != location_service::service_name) {
        return;
    }

    m_locationServiceInterface.connection().connect(
        m_locationServiceInterface.service(),
        m_locationServiceInterface.path(),
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
    Q_EMIT reportWifiAndCellIdsChanged();
}

bool Diagnostics::canReportCrashes()
{
    if (m_whoopsieInterface.isValid()) {
        return m_whoopsieInterface.property("ReportCrashes").toBool();
    }
    return false;
}

bool Diagnostics::canReportWifiAndCellIds()
{
    if (m_locationServiceInterface.isValid()) {
        return m_locationServiceInterface.property("DoesReportCellAndWifiIds").toBool();
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

void Diagnostics::setReportWifiAndCellIds(bool report)
{
    m_locationServiceInterface.property("DoesReportCellAndWifiIds").setValue(report);
}

QString Diagnostics::systemIdentifier() {
    return m_systemIdentifier;
}

Diagnostics::~Diagnostics() {
}
