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

    // For testing, when we want to explicitly set the database path.
    explicit UpdateDb(const QString &dbpath, QObject *parent = nullptr);

    // Add an Update to the database.
    void add(const QSharedPointer<Update> &update);

    /* Update an Update in the database.
     *
     * If it does not exist, it will be created.
     */
    void update(const QSharedPointer<Update> &update);

    // Remove an Update from the database.
    void remove(const QSharedPointer<Update> &update);

    // Return an Update in the database. If not found, the Update will be null.
    // TODO: There's no sharing going on here, maybe drop the shared pointer?
    QSharedPointer<Update> get(const QString &id, const uint &revision);

    // Return all Updates stored in the database.
    QList<QSharedPointer<Update> > updates();

    // Return date of last (successful) check.
    QDateTime lastCheckDate();

    // Set date of last (successful) check.
    void setLastCheckDate(const QDateTime &lastCheck);

    // Enables testing.
    QSqlDatabase db();

    /* Remove all installed Updates older than 30 days.
     *
     * See https://wiki.ubuntu.com/SoftwareUpdates\
     *     #Presenting_available_updates
     */
    void pruneDb();

    // Drops the database. Used in testing.
    void reset();
Q_SIGNALS:
    // This signal is emitted when the database changed substantially.
    void changed();

    // This signal is emitted when an Update changes.
    void changed(const QSharedPointer<Update> &update);
private:
    bool insert(const QSharedPointer<Update> &update);
    static void update(const QSharedPointer<Update> &update,
                       const QSqlQuery &query);
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
