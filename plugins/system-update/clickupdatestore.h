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

namespace UpdatePlugin
{

class ClickUpdateStore : public QObject
{
    Q_OBJECT
public:
    explicit ClickUpdateStore(QObject *parent = 0);
    ~ClickUpdateStore();

    Q_PROPERTY(QSqlQueryModel* installedUpdates READ installedUpdates
               CONSTANT)
    Q_PROPERTY(QSqlQueryModel* activeUpdates READ activeUpdates
               CONSTANT)

    QSqlQueryModel *installedUpdates();
    QSqlQueryModel *activeUpdates();
    QDateTime lastCheckDate();

private slots:
    void clickUpdateInstalled(const QString &uid);

signals:

private:
    void initializeStore();
    bool createDb();

    QSqlDatabase m_db;
    QSqlQueryModel m_installedUpdates;
    QSqlQueryModel m_activeUpdates;
};

} // UpdatePlugin

#endif /* PLUGINS_SYSTEM_UPDATE_CLICKUPDATESTORE_H_ */
