/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLUGINS_SYSTEM_UPDATE_UPDATEMODEL_H
#define PLUGINS_SYSTEM_UPDATE_UPDATEMODEL_H

#include <QSqlQueryModel>
#include "updatestore.h"

namespace UpdatePlugin
{

class UpdateModel : public QSqlQueryModel
{
    Q_OBJECT
    Q_PROPERTY(UpdateTypes filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_ENUMS(UpdateTypes)
public:
    enum class UpdateTypes
    {
        All,
        Pending,
        PendingClicksUpdates,
        PendingSystemUpdates,
        InstalledClicksUpdates,
        InstalledSystemUpdates,
        Installed
    };

    explicit UpdateModel(QObject *parent = 0);
    ~UpdateModel();

    // For testing.
    explicit UpdateModel(const QString &dbpath, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    int count() const;
    void setFilter(const UpdateTypes &filter);
    UpdateTypes filter() const;

public slots:
    // Re-runs query based on the current filter, used in testing.
    Q_INVOKABLE void update();

signals:
    void countChanged();
    void filterChanged();

private:
    void initialize();
    const static char* COLUMN_NAMES[];
    UpdateTypes m_filter;
    UpdateStore *m_store;
};

} // UpdatePlugin

#endif // PLUGINS_SYSTEM_UPDATE_UPDATEMODEL_H
