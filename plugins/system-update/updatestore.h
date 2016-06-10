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

#ifndef PLUGINS_SYSTEM_UPDATE_UPDATESTORE_H
#define PLUGINS_SYSTEM_UPDATE_UPDATESTORE_H

#include <QDateTime>
#include <QDebug>
#include <QSqlDatabase>

#include "clickupdatemetadata.h"
#include "updatemodel.h"

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

    Q_PROPERTY(UpdateModel* installedUpdates READ installedUpdates
               CONSTANT)
    Q_PROPERTY(UpdateModel* pendingClickUpdates READ pendingClickUpdates
               CONSTANT)

    UpdateModel *installedUpdates();
    UpdateModel *pendingClickUpdates();
    QDateTime lastCheckDate();
    void setLastCheckDate(const QDateTime &lastCheckUtc);

    // Adds a click update to the store based on the given metadata.
    // Can safely be called multiple times for the same update.
    void add(const ClickUpdateMetadata *meta);

    // Mark an update as installed.
    void markInstalled(const QString &appId, const int &revision);

    // void setUdmId(const QString &appId, const int &revision, const int &udmId);
    // void unsetUdmId(const QString &appId, const int &revision);

    // void unsetUdmId(const int &udmId);

    // Removes old updates.
    void pruneDb();

private slots:
    void queryPending();
    void queryInstalled();
    void queryAll();

signals:

private:
    void init(const QString &dbpath, QObject *parent);
    void initializeStore();
    bool createDb();
    bool openDb();

    QSqlDatabase m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
    UpdateModel m_installedUpdates;
    UpdateModel m_pendingClickUpdates;
    QString m_dbpath;

    const QString KIND_CLICK = QLatin1String("click");
    const QString KIND_SYSTEM = QLatin1String("system");
};

} // UpdatePlugin

#endif /* PLUGINS_SYSTEM_UPDATE_UPDATESTORE_H */
