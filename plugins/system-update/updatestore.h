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

#ifndef UPDATE_STORE_H
#define UPDATE_STORE_H

#include <QDateTime>
#include <QDebug>
#include <QSqlDatabase>

#include "systemupdate.h"
#include "clickupdatemetadata.h"

namespace UpdatePlugin
{
class UpdateStore : public QObject
{
    Q_OBJECT
public:
    explicit UpdateStore(QObject *parent = 0);
    ~UpdateStore();

    // For testing purposes.
    explicit UpdateStore(const QString &dbpath, QObject *parent = 0);

    QDateTime lastCheckDate();
    void setLastCheckDate(const QDateTime &lastCheckUtc);

    // Adds a click update to the store based on the given metadata.
    // Can safely be called multiple times for the same update.
    void add(const ClickUpdateMetadata *meta);
    ClickUpdateMetadata* getPending(const QString &id);

    void add(const QString &kind, const QString &id,
             const int &revision, const QString &version,
             const QString &changelog, const QString &title,
             const QString &iconUrl, const int &size,
             const bool automatic);

    // Mark an update as installed.
    void markInstalled(const QString &id, const int &revision);
    void setUpdateState(const QString &id, const int &revision,
                        const SystemUpdate::UpdateState &state);
    void setProgress(const QString &id, const int &revision,
                     const int &progress);

    // Removes (30 days) old updates.
    void pruneDb();

    QSqlDatabase db() const;
    bool openDb();

    static const QString KIND_CLICK;
    static const QString KIND_SYSTEM;
    static const QString STATE_PENDING;
    static const QString STATE_INSTALLED;
    QString updateStateToString(const SystemUpdate::UpdateState &state);
    SystemUpdate::UpdateState stringToUpdateState(const QString &state);

signals:
    void changed();
    void itemChanged(const QString &id, const int &revision);

private:
    void init(const QString &dbpath, QObject *parent);
    void initializeStore();
    bool createDb();

    QSqlDatabase m_db;
    QString m_dbpath;
    QString m_connectionName;
};
} // UpdatePlugin

#endif // UPDATE_STORE_H
