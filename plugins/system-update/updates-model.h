/*
 * Copyright (C) 2016 Canonical Ltd
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
#ifndef UPDATESMODEL_H
#define UPDATESMODEL_H

#include <QObject>
#include <QAbstractListModel>


class UpdatesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Update::UpdateType filter READ filter WRITE setFilter NOTIFY filterChanged)

public:
    explicit UpdatesModel(QObject *parent = 0);
    ~UpdatesModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void setFilter(Update::UpdateType filter);
    Update::UpdateType filter();

    Q_INVOKABLE Update *get(int index) const;
    QHash<int, QByteArray> roleNames() const;


Q_SIGNALS:
    void added(Update *update);
    void removed();
    void filterChanged(Update::UpdateType);

private:
    QList<Update *> updates;
    Update::UpdateType currentFilter;

};

#endif // UPDATESMODEL_H
