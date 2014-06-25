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

#ifndef TIMEZONELOCATIONMODEL_H
#define TIMEZONELOCATIONMODEL_H

#include <QAbstractTableModel>
#include <QSet>
#include <QThread>

#include <QtConcurrent>

class TimeZonePopulateWorker;

class TimeZoneLocationModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TimeZoneLocationModel(QObject *parent = 0);
    ~TimeZoneLocationModel();

    enum Roles {
        TimeZoneRole = Qt::UserRole + 1,
        CityRole,
        CountryRole,
        SimpleRole
    };

    struct TzLocation {
        bool operator<(const TzLocation &other) const
        {
            QString pattern("%1, %2");

            return pattern.arg(city).arg(country) <
                    pattern.arg(other.city).arg(other.country);
        }

        QString city;
        QString country;
        QString timezone;
        QString state;
        QString full_country;
    };

    void filter(const QString& pattern);

    // implemented virtual methods from QAbstractTableModel
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    bool modelUpdating;

Q_SIGNALS:
    void filterBegin();
    void filterComplete();
    void modelUpdated();

public Q_SLOTS:
    void processModelResult(TzLocation);
    void store();
    void filterFinished();

private:
    QList<TzLocation> m_locations;
    QList<TzLocation> m_originalLocations;
    QString m_pattern;

    TimeZonePopulateWorker *m_workerThread;

    bool substringFilter(const QString& input);
    QFutureWatcher<TzLocation> m_watcher;
    void setModel(QList<TzLocation> locations);
};

Q_DECLARE_METATYPE (TimeZoneLocationModel::TzLocation)

class TimeZonePopulateWorker : public QThread
{
    Q_OBJECT

public:
    void run();

Q_SIGNALS:
    void resultReady(TimeZoneLocationModel::TzLocation);

private:
    void buildCityMap();

};

#endif // TIMEZONELOCATIONMODEL_H
