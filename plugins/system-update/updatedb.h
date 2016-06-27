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
    Q_ENUMS(Filter)
public:
    enum class Filter
    {
        All,
        Pending,
        PendingReversed,
        PendingClicks,
        PendingImage,
        InstalledClicks,
        InstalledImage,
        Installed
    };

    explicit UpdateDb(QObject *parent = 0);
    ~UpdateDb();
    // For testing.
    explicit UpdateDb(const QString &dbpath, QObject *parent = 0);

    void add(const QSharedPointer<Update> &update);
    void remove(const QSharedPointer<Update> &update);
    // QList<QSharedPointer<ClickUpdate> > getClicks();

    QList<QSharedPointer<Update> > updates(const Filter &filter);

    QSqlDatabase db(); // For testing.

    QDateTime lastCheckDate();
    void setLastCheckDate(const QDateTime &lastCheckUtc);

    void setState(const QString &downloadId,
                  const Update::State &state);
    void setInstalled(const QString &downloadId);
    void setError(const QString &downloadId, const QString &msg);
    void setProgress(const QString &downloadId,
                     const int &progress);
    void setStarted(const QString &downloadId);
    void setProcessing(const QString &downloadId);
    void setPaused(const QString &downloadId);
    void setResumed(const QString &downloadId);
    void setCanceled(const QString &downloadId);
    void setDownloadId(const QString &id, const int &revision,
                       const QString &downloadId);
    // void setState(const QString &downloadId,
    //               const SystemUpdate::UpdateState &state);
    void unsetDownloadId(const QString &downloadId);

    void pruneDb();

signals:
    void changed();

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
