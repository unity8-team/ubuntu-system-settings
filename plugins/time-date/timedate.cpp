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

#include "timedate.h"
#include <QDBusReply>
#include <QEvent>
#include <unistd.h>

TimeDate::TimeDate(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_timeDateInterface ("org.freedesktop.timedate1",
                         "/org/freedesktop/timedate1",
                         "org.freedesktop.timedate1",
                          m_systemBusConnection)
{
    if (!m_timeDateInterface.isValid()) {
        return;
    }

    m_timeDateInterface.connection().connect(
        m_timeDateInterface.service(),
        m_timeDateInterface.path(),
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(slotChanged(QString, QVariantMap, QStringList)));

}

QString TimeDate::timeZone()
{
    if (m_currentTimeZone.isEmpty() || m_currentTimeZone.isNull())
        m_currentTimeZone = getTimeZone();

     return m_currentTimeZone;
}

QString TimeDate::getTimeZone()
{

    if (m_timeDateInterface.isValid()) {
        return m_timeDateInterface.property("Timezone").toString();
    }

    return QString();
}

#define UNUSED __attribute__((__unused__))

void TimeDate::slotChanged(QString UNUSED interface,
                           QVariantMap UNUSED changed_properties,
                           QStringList invalidated_properties)
{
    if (invalidated_properties.contains("Timezone")) {
        m_currentTimeZone = getTimeZone();
        Q_EMIT timeZoneChanged();
    }
}

void TimeDate::setTimeZone(QString &time_zone)
{
    m_timeDateInterface.call("SetTimezone", time_zone, false);
}

TimeDate::~TimeDate() {
}
