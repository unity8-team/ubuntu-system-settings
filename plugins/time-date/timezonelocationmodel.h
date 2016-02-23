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

extern "C"
{
#include <geonames.h>
}

class TimeZoneLocationModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TimeZoneLocationModel(QObject *parent = 0);
    ~TimeZoneLocationModel();

    enum Roles {
        TimeZoneRole = Qt::UserRole + 1,
        CityRole,
        CountryRole,
        SimpleRole
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

private:
    QList<GeonamesCity *> m_locations;
    GCancellable *m_cancellable;

    static void filterFinished(GObject      *source_object,
                               GAsyncResult *res,
                               gpointer      user_data);
    void setModel(QList<GeonamesCity *> locations);
};

#endif // TIMEZONELOCATIONMODEL_H
