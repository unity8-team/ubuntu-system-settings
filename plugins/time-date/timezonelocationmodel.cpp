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
#include <timezonemap/tz.h>

#include <QDebug>

TimeZoneLocationModel::TimeZoneLocationModel(QObject *parent):
    QAbstractTableModel(parent),
    modelUpdating(true),
    m_pattern(),
    m_workerThread(new QThread()),
    m_populateWorker(new TimeZonePopulateWorker()),
    m_watcher()
{
    qRegisterMetaType<TzLocation>();
    qRegisterMetaType<QList<TimeZoneLocationModel::TzLocation> >();

    QObject::connect(m_workerThread,
            SIGNAL(started()),
            m_populateWorker,
            SLOT(doBuild()));
    QObject::connect(m_populateWorker,
            &TimeZonePopulateWorker::resultReady,
            this,
            &TimeZoneLocationModel::store);
    QObject::connect(m_populateWorker,
            SIGNAL(finished()),
            this,
            SLOT(deleteLater()));
    QObject::connect(m_workerThread,
            SIGNAL(finished()),
            m_workerThread,
            SLOT(deleteLater()));

    m_populateWorker->moveToThread(m_workerThread);
    m_workerThread->start(QThread::IdlePriority);
}

void TimeZoneLocationModel::store(QList<TzLocation> sortedLocations)
{
    qWarning() << "model:store()";
    m_originalLocations = sortedLocations;
    Q_EMIT (modelUpdated());
    m_workerThread = nullptr;
    modelUpdating = false;

    QObject::connect(&m_watcher,
                     &QFutureWatcher<TzLocation>::finished,
                     this,
                     &TimeZoneLocationModel::filterFinished);

    if (!m_pattern.isEmpty())
        filter(m_pattern);
    qWarning() << "model:store() ends";
}

void TimeZoneLocationModel::setModel(QList<TzLocation> locations)
{
    beginResetModel();
    m_locations = locations;
    endResetModel();
    Q_EMIT(filterComplete());
}

void TimeZoneLocationModel::filterFinished()
{
    setModel(m_watcher.future().results());
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

    QString country(tz.full_country.isEmpty() ? tz.country : tz.full_country);

    switch (role) {
    case Qt::DisplayRole:
        if (!tz.state.isEmpty())
           return QVariant(QString("%1, %2, %3").arg(tz.city).arg(tz.state)
                   .arg(country));
        else
            return QVariant(QString("%1, %2").arg(tz.city).arg(country));
        break;
    case SimpleRole:
        return QVariant(QString("%1, %2").arg(tz.city).arg(country));
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

void TimeZoneLocationModel::filter(const QString& pattern)
{
    qWarning() << "model:filter()";
    QList<TzLocation> list;

    if (m_watcher.isRunning())
        m_watcher.cancel();

    if (pattern.isEmpty() || pattern.isNull() ||
            (m_workerThread && m_workerThread->isRunning())) {
        setModel(QList<TzLocation>());
        m_pattern = pattern;
        qWarning() << "model:filter() had something running, exits early";
        return;
    }

    qWarning() << "model:filter() continues..";

    if (!m_pattern.isEmpty() && !m_locations.isEmpty() &&
            pattern.startsWith(m_pattern))
        list = m_locations; // search in the smaller list
    else
        list = m_originalLocations; //search in the whole list

    m_pattern = pattern;

    QFuture<TzLocation> future (QtConcurrent::filtered(
                list,
                [pattern] (const TzLocation& tz) {
        QString display("%1, %2");
        return g_str_match_string (pattern.toStdString().c_str(),
                display.arg(tz.city)
                    .arg(tz.full_country.isEmpty() ? tz.country
                                                   : tz.full_country)
                    .toStdString().c_str(),
                TRUE);
    }));

    Q_EMIT (filterBegin());

    m_watcher.setFuture(future);
    qWarning() << "model:filter() completed";
}

void TimeZonePopulateWorker::doBuild()
{
    qWarning() << "TimeZonePopulateWorker doBuild()";
    buildCityMap();
    Q_EMIT (buildCompleted());
    qWarning() << "TimeZonePopulateWorker doBuild() ends";
}

void TimeZonePopulateWorker::buildCityMap()
{
    QList<TimeZoneLocationModel::TzLocation> locations;
    qWarning() << "populateworker:buildCityMap()";
    TzDB *tzdb = tz_load_db();
    GPtrArray *tz_locations = tz_get_locations(tzdb);

    TimeZoneLocationModel::TzLocation tmpTz;

    for (guint i = 0; i < tz_locations->len; ++i) {
        auto tmp = static_cast<CcTimezoneLocation *>(g_ptr_array_index(tz_locations, i));
        gchar *en_name, *country, *zone, *state, *full_country;
        g_object_get (tmp, "en_name", &en_name,
                           "country", &country,
                           "zone", &zone,
                           "state", &state,
                           "full_country", &full_country,
                           nullptr);
        // There are empty entries in the DB
        if (g_strcmp0(en_name, "") != 0) {
            tmpTz.city = en_name;
            tmpTz.country = country;
            tmpTz.timezone = zone;
            tmpTz.state = state;
            tmpTz.full_country = full_country;
        }
        g_free (en_name);
        g_free (country);
        g_free (zone);
        g_free (state);
        g_free (full_country);

        locations.append(tmpTz);
    }

    qSort(locations.begin(), locations.end());
    Q_EMIT (resultReady(locations));
    qWarning() << "populateworker:buildCityMap() ends";
    g_ptr_array_free (tz_locations, TRUE);
}

TimeZoneLocationModel::~TimeZoneLocationModel()
{
}
