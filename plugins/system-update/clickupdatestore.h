/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PLUGINS_SYSTEM_UPDATE_CLICKUPDATESTORE_H_
#define PLUGINS_SYSTEM_UPDATE_CLICKUPDATESTORE_H_

#include <QDateTime>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>

#include "clickupdatemetadata.h"

namespace UpdatePlugin
{

class ClickUpdateStore : public QObject
{
    Q_OBJECT
public:
    explicit ClickUpdateStore(QObject *parent = 0);
    ~ClickUpdateStore();

    // For testing purposes.
    explicit ClickUpdateStore(const QString &dbpath, QObject *parent = 0);

    Q_PROPERTY(QSqlQueryModel* installedUpdates READ installedUpdates
               CONSTANT)
    Q_PROPERTY(QSqlQueryModel* activeUpdates READ activeUpdates
               CONSTANT)

    QSqlQueryModel *installedUpdates();
    QSqlQueryModel *activeUpdates();
    QDateTime lastCheckDate();
    void setLastCheckDate(const QDateTime &lastCheckUtc);

    // Adds a click update to the store based on the given metadata.
    // Can safely be called multiple times for the same update.
    void add(const ClickUpdateMetadata *meta);

    // Mark a click update as installed.
    void markInstalled(const QString &appId, const int &revision);

    void setUdmId(const QString &appId, const int &revision, const int &udmId);
    void unsetUdmId(const QString &appId, const int &revision);

private slots:
    void queryActive();
    void queryInstalled();
    void queryAll();

signals:

private:
    void initializeStore();
    bool createDb();
    bool openDb();

    // Removes old updates.
    void pruneDb();

    QSqlDatabase m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
    QSqlQueryModel m_installedUpdates;
    QSqlQueryModel m_activeUpdates;
    QString m_dbpath;
};

} // UpdatePlugin

#endif /* PLUGINS_SYSTEM_UPDATE_CLICKUPDATESTORE_H_ */
