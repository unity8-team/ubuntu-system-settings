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
#include <QDebug>
#include <QEvent>
#include <glib.h>
#include <glib-object.h>
#include <timezonemap/tz.h>
#include <unistd.h>

TimeDate::TimeDate(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_serviceWatcher ("org.freedesktop.timedate1",
                      m_systemBusConnection,
                      QDBusServiceWatcher::WatchForOwnerChange),
    m_timeDateInterface ("org.freedesktop.timedate1",
                         "/org/freedesktop/timedate1",
                         "org.freedesktop.timedate1",
                          m_systemBusConnection),
    m_timeZoneModel(),
    m_timeZoneFilterProxy(this)
{
    connect (&m_serviceWatcher,
             SIGNAL (serviceOwnerChanged (QString, QString, QString)),
             this,
             SLOT (slotNameOwnerChanged (QString, QString, QString)));

    if (m_timeDateInterface.isValid()) {
        setUpInterface();
    }

}

void TimeDate::setUpInterface()
{
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

void TimeDate::slotChanged(QString interface,
                           QVariantMap changed_properties,
                           QStringList invalidated_properties)
{
    Q_UNUSED (interface);
    Q_UNUSED (changed_properties);

    if (invalidated_properties.contains("Timezone")) {
        m_currentTimeZone = getTimeZone();
        Q_EMIT timeZoneChanged();
    }
}

void TimeDate::slotNameOwnerChanged(QString name,
                                    QString oldOwner,
                                    QString newOwner)
{
    Q_UNUSED (oldOwner);
    Q_UNUSED (newOwner);
    if (name != "org.freedesktop.timedate1")
        return;

    setUpInterface();
    // Tell QML so that it refreshes its view of the property
    Q_EMIT timeZoneChanged();
}

void TimeDate::setTimeZone(QString &time_zone)
{
    m_timeDateInterface.call("SetTimezone", time_zone, false);
}

QAbstractItemModel *TimeDate::getTimeZoneModel()
{
    m_timeZoneFilterProxy.setSourceModel(&m_timeZoneModel);
    m_timeZoneFilterProxy.setDynamicSortFilter(false);
    // By default don't display anything
    m_timeZoneFilterProxy.setFilterRegExp("^$");
    m_timeZoneFilterProxy.setFilterCaseSensitivity(Qt::CaseInsensitive);
    return &m_timeZoneFilterProxy;
}

QString TimeDate::getFilter()
{
    return m_filter;
}

void TimeDate::setFilter(QString &new_filter)
{
    // Empty string should match nothing
    if (new_filter.isEmpty())
        new_filter = "^$";
    m_filter = new_filter;
    m_timeZoneFilterProxy.setFilterRegExp(new_filter);
}

TimeDate::~TimeDate() {
}
