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
#include "updatemodel.h"

namespace UpdatePlugin
{

UpdateModel::UpdateModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

UpdateModel::~UpdateModel()
{
}

QHash<int, QByteArray> UpdateModel::roleNames() const
{
    int idx = 0;
    QHash<int, QByteArray> roleNames;
    while (COLUMN_NAMES[idx]) {
        roleNames[Qt::UserRole + idx + 1] = COLUMN_NAMES[idx];
        idx++;
    }
    return roleNames;
}

QVariant UpdateModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
    if (role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    } else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

const char* UpdateModel::COLUMN_NAMES[] = {
    "kind",
    "app_id",
    "local_version",
    "remote_version",
    "revision",
    "state",
    "created_at_utc",
    "updated_at_utc",
    "title",
    "download_sha512",
    "size",
    "icon_url",
    "download_url",
    "changelog",
    "udm_download_id",
    "click_token",
    "command",
    NULL
};

} // UpdatePlugin
