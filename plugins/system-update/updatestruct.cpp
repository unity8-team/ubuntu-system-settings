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
    kind = query->value("kind").toString();
    id = query->value("id").toString();
    localVersion = query->value("local_version").toString();
    remoteVersion = query->value("remote_version").toString();
    revision = query->value("revision").toInt();
    installed = query->value("installed").toBool();
    createdAt = QDateTime::fromMSecsSinceEpoch(
        query->value("created_at_utc").toLongLong()
    );
    updatedAt = QDateTime::fromMSecsSinceEpoch(
        query->value("updated_at_utc").toLongLong()
    );
    title = query->value("title").toString();
    downloadHash = query->value("download_sha512").toString();
    size = query->value("size").toInt();
    iconUrl = query->value("icon_url").toString();
    downloadUrl = query->value("download_url").toString();
    command = query->value("command").toString().split(" ");
    changelog = query->value("changelog").toString();
    token = query->value("token").toString();
    updateState = Update::stringToState(
        query->value("update_state").toString()
    );
    progress = query->value("progress").toInt();
    automatic = query->value("automatic").toBool();
    downloadId = query->value("download_id").toString();
    error = query->value("error").toString();
}

bool UpdateStruct::operator==(const UpdateStruct &other) const
{
    if (other.id == id && other.revision == revision)
        return true;
    else if (!downloadId.isEmpty() && (other.downloadId == downloadId))
        return true;
    else
        return false;
}

bool UpdateStruct::deepEquals(const UpdateStruct &other) const
{
    if (kind != other.kind) return false;
    if (id != other.id) return false;
    if (localVersion != other.localVersion) return false;
    if (remoteVersion != other.remoteVersion) return false;
    if (revision != other.revision) return false;
    if (installed != other.installed) return false;
    if (createdAt != other.createdAt) return false;
    if (updatedAt != other.updatedAt) return false;
    if (title != other.title) return false;
    if (downloadHash != other.downloadHash) return false;
    if (size != other.size) return false;
    if (iconUrl != other.iconUrl) return false;
    if (downloadUrl != other.downloadUrl) return false;
    if (command != other.command) return false;
    if (changelog != other.changelog) return false;
    if (token != other.token) return false;
    if (updateState != other.updateState) return false;
    if (progress != other.progress) return false;
    if (automatic != other.automatic) return false;
    if (downloadId != other.downloadId) return false;
    if (error != other.error) return false;

    return true;
}
} // UpdatePlugin
