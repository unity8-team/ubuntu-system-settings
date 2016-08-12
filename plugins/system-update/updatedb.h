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

#ifndef UPDATE_DB_H
#define UPDATE_DB_H

#include "update.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QSqlDatabase>

namespace UpdatePlugin
{
class UpdateDb : public QObject
{
    Q_OBJECT
public:
    explicit UpdateDb(QObject *parent = nullptr);
    ~UpdateDb();
    explicit UpdateDb(const QString &dbpath, QObject *parent = nullptr);
    void add(const QSharedPointer<Update> &update);
    void update(const QSharedPointer<Update> &update);
    void remove(const QSharedPointer<Update> &update);
    QSharedPointer<Update> get(const QString &id, const uint &revision);
    QList<QSharedPointer<Update> > updates();
    QDateTime lastCheckDate();
    void setLastCheckDate(const QDateTime &lastCheck);
    QSqlDatabase db();

    /* Remove all installed Updates older than 30 days.
     *
     * See https://wiki.ubuntu.com/SoftwareUpdates\
     *     #Presenting_available_updates
     */
    void pruneDb();
    void reset();
    const uint SCHEMA_VERSION = 1;
Q_SIGNALS:
    // This signal is emitted when multiple rows changed.
    void changed();
    // This signal is emitted when a single Update changes.
    void changed(const QSharedPointer<Update> &update);
private:
    bool insert(const QSharedPointer<Update> &update);
    static void update(const QSharedPointer<Update> &update,
                       const QSqlQuery &query);
    bool dropDb();
    bool migrateDb();
    bool createDb();
    void initializeDb();
    bool openDb();
    // Removes any updates that precede update and are not installed.
    void replaceWith(const QSharedPointer<Update> &update);
    QSqlDatabase m_db;
    QString m_dbpath;
    QString m_connectionName;
};
} // UpdatePlugin

#endif // UPDATE_DB_H
