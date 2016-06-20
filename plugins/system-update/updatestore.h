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

    void add(const QString &kind, const QString &uniqueIdentifier,
             const int &revision, const QString &version,
             const QString &changelog, const QString &title,
             const QString &iconUrl, const int &size);

    // Mark an update as installed.
    void markInstalled(const QString &uniqueIdentifier, const int &revision);

    // Removes (30 days) old updates.
    void pruneDb();

    QSqlDatabase db() const;
    bool openDb();

    const QString KIND_CLICK = QLatin1String("click");
    const QString KIND_SYSTEM = QLatin1String("system");

signals:
    void updatesChanged();

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
