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

#include "systemupdate.h"
#include "updatedb.h"
#include "updatemodel.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QStandardPaths>

namespace UpdatePlugin
{
UpdateModel::UpdateModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_db(SystemUpdate::instance()->updateDb())
    , m_filter(Update::Filter::All)
    , m_updates()
{
    initialize();
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QAbstractListModel(parent)
    , m_db(new UpdateDb(dbpath, this))
    , m_filter(Update::Filter::All)
    , m_updates()
{
    initialize();
}

void UpdateModel::initialize()
{
    // connect(this, SIGNAL(filterChanged()), SLOT(refresh()));
    // connect(this, SIGNAL(changed()),
    //         SystemUpdate::instance(), SLOT(notifyModelChanged()));
    // connect(this, SIGNAL(updateChanged(QString, int)),
    //         SystemUpdate::instance(), SLOT(notifyModelItemChanged(QString, int)));
    // connect(this, SIGNAL(updateChanged(QString)),
    //         SystemUpdate::instance(), SLOT(notifyModelItemChanged(QString)));

    // connect(SystemUpdate::instance(), SIGNAL(modelChanged()),
    //         this, SLOT(refresh()));
    // connect(SystemUpdate::instance(), SIGNAL(modelItemChanged(QString, int)),
    //         this, SLOT(refreshItem(QString, int)));
    // connect(SystemUpdate::instance(), SIGNAL(modelItemChanged(QString)),
    //         this, SLOT(refreshItem(QString)));
}

QHash<int, QByteArray> UpdateModel::roleNames() const
{
    static QHash<int,QByteArray> names;

    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[KindRole] = "kind";
        names[IconUrlRole] = "iconUrl";
        names[IdRole] = "identifier";
        names[LocalVersionRole] = "localVersion";
        names[RemoteVersionRole] = "remoteVersion";
        names[RevisionRole] = "revision";
        names[InstalledRole] = "installed";
        names[CreatedAtRole] = "createdAt";
        names[UpdatedAtRole] = "updatedAt";
        names[TitleRole] = "title";
        names[DownloadHashRole] = "downloadHash";
        names[SizeRole] = "size";
        names[DownloadUrlRole] = "downloadUrl";
        names[ChangelogRole] = "changelog";
        names[CommandRole] = "command";
        names[TokenRole] = "token";
        names[UpdateStateRole] = "updateState";
        names[ProgressRole] = "progress";
        names[AutomaticRole] = "automatic";
        names[DownloadIdRole] = "downloadId";
    }

    return names;
}

UpdateDb* UpdateModel::db()
{
    return m_db;
}

QVariant UpdateModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    if (row < 0 || row > (m_updates.length() - 1))
        return QVariant();

    UpdateStruct u = m_updates.at(row);

    switch (role) {
    case Qt::DisplayRole:
    case KindRole:
        return u.kind;
    case IdRole:
        return u.id;
    case LocalVersionRole:
        return u.localVersion;
    case RemoteVersionRole:
        return u.remoteVersion;
    case RevisionRole:
        return u.revision;
    case InstalledRole:
        return u.installed;
    case CreatedAtRole:
        return u.createdAt;
    case UpdatedAtRole:
        return u.updatedAt;
    case TitleRole:
        return u.title;
    case DownloadHashRole:
        return u.downloadHash;
    case SizeRole:
        return u.size;
    case IconUrlRole:
        return u.iconUrl;
    case DownloadUrlRole:
        return u.downloadUrl;
    case CommandRole:
        return u.command;
    case ChangelogRole:
        return u.changelog;
    case TokenRole:
        return u.token;
    case UpdateStateRole:
        return (int) u.updateState;
    case ProgressRole:
        return u.progress;
    case AutomaticRole:
        return u.automatic;
    case DownloadIdRole:
        return u.downloadId;
    case ErrorRole:
        return u.error;
    }
    return QVariant();
}

int UpdateModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_updates.size();
}

int UpdateModel::count() const
{
    return rowCount();
}

void UpdateModel::setFilter(const Update::Filter &filter)
{
    if (filter != m_filter) {
        m_filter = filter;
        Q_EMIT (filterChanged());
    }
}

Update::Filter UpdateModel::filter() const
{
    return m_filter;
}

// void UpdateModel::refresh()
// {
//     qWarning() << "blast refresh...";
//     int oldCount = count();
//     if (!openDb())
//         qWarning() << "could not open db";

//     QSqlQuery q(m_db);
//     switch (m_filter) {
//     case Update::Filter::All:
//         // q = QSqlQuery(GET_ALL, m_db);
//         q.prepare(GET_ALL);
//         break;
//     case Update::Filter::Pending:
//     case Update::Filter::PendingClicksUpdates:
//         q.prepare(GET_PENDING_CLICKS);
//         q.bindValue(":installed", false);
//         q.bindValue(":kind", UpdateModel::KIND_CLICK);
//         break;
//     case Update::Filter::PendingSystemUpdates:
//         return; // We don't store these, so bail.
//     case Update::Filter::InstalledClicksUpdates:
//         q.prepare(GET_INSTALLED_CLICKS);
//         q.bindValue(":installed", true);
//         q.bindValue(":kind", UpdateModel::KIND_CLICK);
//         break;
//     case Update::Filter::InstalledSystemUpdates:
//         q.prepare(GET_INSTALLED_SYSUPDATES);
//         q.bindValue(":installed", true);
//         q.bindValue(":kind", UpdateModel::KIND_SYSTEM);
//         break;
//     case Update::Filter::Installed:
//         q.prepare(GET_INSTALLED);
//         q.bindValue(":installed", true);
//         break;
//     }

//     if (!q.exec()) {
//         qCritical() << "could not update" << q.lastError().text() << q.executedQuery();
//         return;
//     }

//     // m_updates.clear();
//     // beginResetModel();

//     QList<UpdateStruct> listNow;
//     // QList<UpdateStruct> changed;
//     // QList<UpdateStruct> removed;
//     QList<UpdateStruct> added;

//     while (q.next()) {
//         UpdateStruct u;
//         u.setValues(&q);
//         listNow.append(u);

//         if (m_updates.contains(u)) {
//             int ix = m_updates.indexOf(u);
//             if (!u.deepEquals(m_updates.at(ix))) {
//                 m_updates.replace(ix, u);
//                 Q_EMIT (dataChanged(index(ix, 0), index(ix, 0)));
//             }
//         } else {
//             added.append(u);
//         }
//     }

//     for (int i = 0; i < m_updates.size(); i++) {

//     }

//     // endResetModel();

//     if (oldCount != count()) {
//         Q_EMIT (countChanged());
//     }
// }

// void UpdateModel::refreshItem(const QString &id, const int &revision)
// {
//     // qWarning() << Q_FUNC_INFO << id << revision;
//     int idx = find(id, revision);

//     if (idx < 0) {
//         return;
//     }

//     // qWarning() << Q_FUNC_INFO << "found something";
//     refresh(idx);
// }

// void UpdateModel::refreshItem(const QString &downloadId)
// {
//     // qWarning() << Q_FUNC_INFO << downloadId;
//     int idx = find(downloadId);

//     if (idx < 0) {
//         return;
//     }

//     // qWarning() << Q_FUNC_INFO << "found some id";
//     refresh(idx);
// }

// void UpdateModel::refresh(const int &idx)
// {
//     // UpdateStruct update = m_updates[idx];
//     // // qWarning() << Q_FUNC_INFO << idx << update.id << update.revision;
//     // if (!openDb()) return;
//     // QSqlQuery q(m_db);
//     // q.prepare(GET_SINGLE);
//     // q.bindValue(":id", update.id);
//     // q.bindValue(":revision", update.revision);

//     // if (!q.exec()) {
//     //     qCritical() << "could fetch item from db" << q.lastError().text();
//     //     return;
//     // }

//     // q.next();

//     // update.setValues(&q);
//     // m_updates.replace(idx, update);

//     // QModelIndex first = index(idx, 0);
//     // QModelIndex last = index(first.row(), 0);

//     // // qWarning() << Q_FUNC_INFO << "datachanged" << idx << first << last;
//     // Q_EMIT (dataChanged(first, last));
// }

void UpdateModel::setInstalled(const QString &downloadId)
{

}

void UpdateModel::startUpdate(const QString &downloadId)
{
}

void UpdateModel::processUpdate(const QString &downloadId)
{
}

void UpdateModel::setError(const QString &downloadId, const QString &msg)
{

}

// void UpdateModel::setState(const QString &downloadId,
//                            const SystemUpdate::UpdateState &state)
// {

// }

void UpdateModel::setProgress(const QString &downloadId,
                              const int &progress)
{

}

void UpdateModel::setDownloadId(const QString &id, const int &revision,
                                const QString &downloadId)
{

}

void UpdateModel::pauseUpdate(const QString &downloadId)
{

}

void UpdateModel::resumeUpdate(const QString &downloadId)
{

}

void UpdateModel::cancelUpdate(const QString &downloadId)
{

}

int UpdateModel::find(const QString &id, const int &revision) const
{
    int res = -1;
    for (int i = 0; i < m_updates.size(); i++) {
        if (m_updates.at(i).id == id && m_updates.at(i).revision == revision) {
            return i;
        }
    }
    return res;
}

int UpdateModel::find(const QString &downloadId) const
{
    int res = -1;
    for (int i = 0; i < m_updates.size(); i++) {
        if (m_updates.at(i).downloadId == downloadId) {
            return i;
        }
    }
    return res;
}

bool UpdateModel::contains(const QString& downloadId) const
{
    for (int i = 0; i < m_updates.size(); i++) {
        if (m_updates.at(i).downloadId == downloadId) {
            return true;
        }
    }
    return false;
}
} // UpdatePlugin
