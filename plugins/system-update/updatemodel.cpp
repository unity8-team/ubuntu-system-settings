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
#include <QSqlError>

#include "updatemodel.h"
#include "updatemanager.h"

namespace UpdatePlugin
{

UpdateModel::UpdateModel(QObject *parent)
    : QSqlQueryModel(parent)
    , m_filter(UpdateTypes::All)
    , m_store(UpdateManager::instance()->updateStore())
{
    initialize();
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QSqlQueryModel(parent)
    , m_filter(UpdateTypes::All)
    , m_store(new UpdateStore(dbpath, this))
{
    initialize();
}

void UpdateModel::initialize()
{
    connect(this, SIGNAL(filterChanged()), SLOT(update()));
    connect(m_store, &UpdateStore::updatesChanged, this, &UpdateModel::update);
}

UpdateModel::~UpdateModel()
{
}

QHash<int, QByteArray> UpdateModel::roleNames() const
{
    int idx = 0;
    QHash<int, QByteArray> roleNames;
    while (COLUMN_NAMES[idx]) {
        roleNames[Qt::UserRole + idx + 1] = COLUMN_NAMES[idx];
        idx++;
    }
    return roleNames;
}

QVariant UpdateModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
    if (role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    } else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

int UpdateModel::count() const
{
    return rowCount();
}


void UpdateModel::setFilter(const UpdateModel::UpdateTypes &filter)
{
    if (filter != m_filter) {
        m_filter = filter;
        Q_EMIT (filterChanged());
    }
}

UpdateModel::UpdateTypes UpdateModel::filter() const
{
    return m_filter;
}

void UpdateModel::update()
{
    if (!m_store->openDb()) return;

    QString sql;
    switch (m_filter) {
    case UpdateTypes::All:
        sql = "SELECT * FROM updates";
        break;
    case UpdateTypes::Pending:
    case UpdateTypes::PendingClicksUpdates:
        sql = "SELECT *, MAX(revision) FROM updates WHERE state='pending' "
              " AND kind='" + m_store->KIND_CLICK + "' GROUP BY app_id"
              " ORDER BY title ASC";
        break;
    case UpdateTypes::PendingSystemUpdates:
        sql = ""; // We don't store it, use SI instead.
        qWarning() << "Ignoring filter for pending system updates.";
        break;
    case UpdateTypes::InstalledClicksUpdates:
        sql = "SELECT * FROM updates WHERE state='installed'"
              " AND kind='" + m_store->KIND_CLICK + "' ORDER BY updated_at_utc DESC";
        break;
    case UpdateTypes::InstalledSystemUpdates:
        sql = "SELECT * FROM updates WHERE state='installed'"
              " AND kind='" + m_store->KIND_SYSTEM + "' ORDER BY updated_at_utc DESC";
        break;
    case UpdateTypes::Installed:
        sql = "SELECT * FROM updates WHERE state='installed'";
        break;
    }

    if (!sql.isEmpty())
        setQuery(sql, m_store->db());
        if (lastError().isValid())
            qWarning() << lastError();

    m_store->db().close();
}

const char* UpdateModel::COLUMN_NAMES[] = {
    "kind",
    "app_id",
    "local_version",
    "remote_version",
    "revision",
    "state",
    "created_at_utc",
    "updated_at_utc",
    "title",
    "download_sha512",
    "size",
    "icon_url",
    "download_url",
    "changelog",
    "udm_download_id",
    "click_token",
    "command",
    NULL
};

} // UpdatePlugin
