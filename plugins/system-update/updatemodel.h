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
 */
#ifndef PLUGINS_SYSTEM_UPDATE_UPDATEMODEL_H
#define PLUGINS_SYSTEM_UPDATE_UPDATEMODEL_H

#include <QSqlQueryModel>

namespace UpdatePlugin
{

class UpdateModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit UpdateModel(QObject *parent = 0);
    ~UpdateModel();

    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

private:
    const static char* COLUMN_NAMES[];
    QString QUERY;
};

} // UpdatePlugin

#endif // PLUGINS_SYSTEM_UPDATE_UPDATEMODEL_H
