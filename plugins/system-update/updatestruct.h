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

#ifndef UPDATE_STRUCT_H
#define UPDATE_STRUCT_H

#include "systemupdate.h"

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QSqlQuery>

namespace UpdatePlugin
{
class UpdateStruct {
public:
    explicit UpdateStruct();
    ~UpdateStruct() {};

    QString kind;
    QString id;
    QString localVersion;
    QString remoteVersion;
    int revision;
    bool installed;
    QDateTime createdAt;
    QDateTime updatedAt;
    QString title;
    QString downloadHash;
    int size;
    QString iconUrl;
    QString downloadUrl;
    QStringList command;
    QString changelog;
    QString token;
    SystemUpdate::UpdateState updateState;
    int progress;
    bool automatic;
    QString downloadId;
    QString error;

    void setValues(const QSqlQuery *query);
    bool operator==(const UpdateStruct &other) const;
};
} // UpdatePlugin

#endif // UPDATE_STRUCT_H
