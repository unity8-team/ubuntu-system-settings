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

#include "updatemodel.h"
#include "updatestruct.h"

namespace UpdatePlugin
{
UpdateStruct::UpdateStruct()
{
}

void UpdateStruct::setValues(const QSqlQuery *query)
{
    kind = query->value(0).toString();
    id = query->value(1).toString();
    localVersion = query->value(2).toString();
    remoteVersion = query->value(3).toString();
    revision = query->value(4).toInt();
    installed = query->value("installed").toBool();
    createdAt = QDateTime::fromMSecsSinceEpoch(query->value(6).toLongLong());
    updatedAt = QDateTime::fromMSecsSinceEpoch(query->value(7).toLongLong());
    title = query->value(8).toString();
    downloadHash = query->value(9).toString();
    size = query->value(10).toInt();
    iconUrl = query->value(11).toString();
    downloadUrl = query->value(12).toString();
    command = query->value("command").toString().split(" ");
    changelog = query->value(14).toString();
    token = query->value(15).toString();
    updateState = UpdateModel::stringToUpdateState(query->value(16).toString());
    progress = query->value("progress").toInt();
    automatic = query->value(18).toBool();
    downloadId = query->value("download_id").toString();
    error = query->value("error").toString();
}

bool UpdateStruct::operator==(const UpdateStruct &other) const
{
    if (other.id == id && other.revision == revision)
        return true;
    else if (other.downloadId == downloadId)
        return true;
    else
        return false;
}
}
