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

/* Object that wraps the updates database and will normally only be
instantiated once. */
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
    QSharedPointer<Update> get(const QString &id, const int &revision);
    // QList<QSharedPointer<ClickUpdate> > getClicks();

    QList<QSharedPointer<Update> > updates(const uint &filter);

    QSqlDatabase db(); // For testing.

    QDateTime lastCheckDate();
    void setLastCheckDate(const QDateTime &lastCheckUtc);

    void setState(const QString &id, const int &revision,
                  const Update::State &state);
    void setInstalled(const QString &id, const int &revision);
    void setError(const QString &id, const int &revision, const QString &msg);
    void setProgress(const QString &id, const int &revision,
                     const int &progress);
    void setStarted(const QString &id, const int &revision);
    void setQueued(const QString &id, const int &revision);
    void setProcessing(const QString &id, const int &revision);
    void setPaused(const QString &id, const int &revision);
    void setResumed(const QString &id, const int &revision);
    void setCanceled(const QString &id, const int &revision);
    // void setDownloadId(const QString &id, const int &revision,
    //                    const QString &downloadId);
    // void setState(const QString &downloadId,
    //               const SystemUpdate::UpdateState &state);
    // void unsetDownloadId(const QString &downloadId);

    void pruneDb();

signals:
    void changed();
    void changed(const QString &id, const int &revision);

private:
    bool createDb();
    void initializeDb();
    bool openDb();

    QSqlDatabase m_db;
    QString m_dbpath;
    QString m_connectionName;
};
} // UpdatePlugin

#endif // UPDATE_DB_H
