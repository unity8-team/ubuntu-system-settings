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

#include "timezonelocationmodel.h"
#include <glib.h>
#include <glib-object.h>
#include <timezonemap/tz.h>

#include <QDebug>

TimeZoneLocationModel::TimeZoneLocationModel(QObject *parent):
    QAbstractTableModel(parent)
{
    qRegisterMetaType<TzLocation>();

    TimeZonePopulateWorker *workerThread = new TimeZonePopulateWorker();
    QObject::connect(workerThread,
            &TimeZonePopulateWorker::resultReady,
            this,
            &TimeZoneLocationModel::processModelResult);
    QObject::connect(workerThread,
            &TimeZonePopulateWorker::finished,
            workerThread,
            &QObject::deleteLater);
    workerThread->start();
}

void TimeZoneLocationModel::processModelResult(TzLocation location)
{
    QModelIndex root;
    int rows = rowCount();
    beginInsertRows(root, rows, rows);
    m_locations.append(location);
    endInsertRows();
}

int TimeZoneLocationModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_locations.count();
}

int TimeZoneLocationModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 3; //TZ, City, Country
}

QVariant TimeZoneLocationModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= m_locations.count() ||
            index.row() < 0)
        return QVariant();

    TzLocation tz = m_locations[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return QVariant(QString("%1, %2").arg(tz.city).arg(tz.country));
        break;
    case TimeZoneRole:
        return tz.timezone;
        break;
    case CountryRole:
        return tz.country;
        break;
    case CityRole:
        return tz.city;
        break;
    default:
        return QVariant();
        break;
    }
}

QHash<int, QByteArray> TimeZoneLocationModel::roleNames() const
{
    QHash<int, QByteArray> m_roleNames;
    m_roleNames[Qt::DisplayRole] = "displayName";
    m_roleNames[TimeZoneRole] = "timeZone";
    m_roleNames[CityRole] = "city";
    m_roleNames[CountryRole] = "country";

    return m_roleNames;
}

void TimeZonePopulateWorker::run()
{
    buildCityMap();
}

void TimeZonePopulateWorker::buildCityMap()
{
    TzDB *tzdb = tz_load_db();
    GPtrArray *tz_locations = tz_get_locations(tzdb);

    CcTimezoneLocation *tmp;
    TimeZoneLocationModel::TzLocation tmpTz;

    for (guint i = 0; i < tz_locations->len; ++i) {
        tmp = (CcTimezoneLocation *) g_ptr_array_index(tz_locations, i);
        gchar *en_name, *country, *zone;
        g_object_get (tmp, "en_name", &en_name,
                           "country", &country,
                           "zone", &zone,
                           NULL);
        // There are empty entries in the DB
        if (g_strcmp0(en_name, "") != 0) {
            tmpTz.city = en_name;
            tmpTz.country = country;
            tmpTz.timezone = zone;
        }
        g_free (en_name);
        g_free (country);
        g_free (zone);

        Q_EMIT (resultReady(tmpTz));
    }

    g_ptr_array_free (tz_locations, TRUE);
}

TimeZoneLocationModel::~TimeZoneLocationModel()
{
}

TimeZoneFilterProxy::TimeZoneFilterProxy(TimeZoneLocationModel *parent)
    : QSortFilterProxyModel(parent)
{
    this->setSourceModel(parent);
    this->setDynamicSortFilter(true);
    // By default don't display anything
    this->setFilterRegExp("^$");
    this->setFilterCaseSensitivity(Qt::CaseInsensitive);
}
