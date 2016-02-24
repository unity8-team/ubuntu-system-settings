/*
 * Copyright (C) 2013-2014 Canonical Ltd
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

#include <QDebug>

TimeZoneLocationModel::TimeZoneLocationModel(QObject *parent):
    QAbstractTableModel(parent),
    modelUpdating(false),
    m_cancellable(nullptr)
{
}

void TimeZoneLocationModel::setModel(QList<GeonamesCity *> locations)
{
    modelUpdating = false;
    beginResetModel();

    Q_FOREACH(GeonamesCity *city, m_locations) {
        geonames_city_free(city);
    }

    m_locations = locations;
    endResetModel();
    Q_EMIT(filterComplete());
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

    GeonamesCity *city = m_locations[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return QVariant(QString("%1, %2, %3").arg(geonames_city_get_name(city))
                                             .arg(geonames_city_get_state(city))
                                             .arg(geonames_city_get_country(city)));
        break;
    case SimpleRole:
        return QVariant(QString("%1, %2").arg(geonames_city_get_name(city))
                                         .arg(geonames_city_get_country(city)));
        break;
    case TimeZoneRole:
        return geonames_city_get_timezone(city);
        break;
    case CountryRole:
        return geonames_city_get_country(city);
        break;
    case CityRole:
        return geonames_city_get_name(city);
        break;
    default:
        return QVariant();
        break;
    }
    throw "Unreachable code";
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

void TimeZoneLocationModel::filterFinished(GObject      *source_object,
                                           GAsyncResult *res,
                                           gpointer      user_data)
{
    Q_UNUSED(source_object);

    g_autofree gint *cities = nullptr;
    guint cities_len = 0;
    g_autoptr(GError) error = nullptr;

    cities = geonames_query_cities_finish(res, &cities_len, &error);
    if (error) {
        return;
    }

    QList<GeonamesCity *> locations;

    for (guint i = 0; i < cities_len; ++i) {
        GeonamesCity *city = geonames_get_city(cities[i]);
        if (city) {
            locations.append(city);
        }
    }

    TimeZoneLocationModel *model = static_cast<TimeZoneLocationModel *>(user_data);
    model->setModel(locations);
}

void TimeZoneLocationModel::filter(const QString& pattern)
{
    modelUpdating = true;

    Q_EMIT (filterBegin());

    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_object_unref(m_cancellable);
        m_cancellable = nullptr;
    }

    if (pattern.isEmpty()) {
        setModel(QList<GeonamesCity *>());
        return;
    }

    m_cancellable = g_cancellable_new();
    geonames_query_cities(pattern.toUtf8().data(),
                          GEONAMES_QUERY_DEFAULT,
                          m_cancellable,
                          filterFinished,
                          this);
}

TimeZoneLocationModel::~TimeZoneLocationModel()
{
    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_object_unref(m_cancellable);
    }

    Q_FOREACH(GeonamesCity *city, m_locations) {
        geonames_city_free(city);
    }
}
