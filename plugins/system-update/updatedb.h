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

/* Object that wraps the updates database and will only be instantiated
once. */
class UpdateDb : public QObject
{
    Q_OBJECT
public:

    explicit UpdateDb(QObject *parent = 0);
    ~UpdateDb();
    // For testing.
    explicit UpdateDb(const QString &dbpath, QObject *parent = 0);

    void add(const QSharedPointer<Update> &update);
    void remove(const QSharedPointer<Update> &update);
    QSharedPointer<Update> get(const QString &id, const uint &revision);
    QList<QSharedPointer<Update> > updates(const uint &filter = 0);

    QSqlDatabase db(); // For testing.

    QDateTime lastCheckDate();
    void setLastCheckDate(const QDateTime &lastCheck);

    void setInstalled(const QString &id, const uint &revision);
    void setError(const QString &id, const uint &revision, const QString &msg);
    void setDownloaded(const QString &id, const uint &revision);
    void setProgress(const QString &id, const uint &revision,
                     const int &progress);
    void setStarted(const QString &id, const uint &revision);
    void setQueued(const QString &id, const uint &revision);
    void setProcessing(const QString &id, const uint &revision);
    void setPaused(const QString &id, const uint &revision);
    void setResumed(const QString &id, const uint &revision);
    void setCanceled(const QString &id, const uint &revision);

    void pruneDb();

signals:
    void changed();
    void changed(const QString &id, const uint &revision);

private:
    static void update(const QSharedPointer<Update> &update,
                       const QSqlQuery &query);
    void setState(const QString &id, const uint &revision,
                  const Update::State &state);
    bool createDb();
    void initializeDb();
    bool openDb();

    QSqlDatabase m_db;
    QString m_dbpath;
    QString m_connectionName;
};
} // UpdatePlugin

#endif // UPDATE_DB_H
