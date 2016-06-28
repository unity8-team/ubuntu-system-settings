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
    , m_db(SystemUpdate::instance()->db())
    , m_filter((int) UpdateDb::Filter::All)
    , m_updates()
{
    initialize();
    // connect(SystemUpdate::instance(), SIGNAL(dbChanged()), this, SLOT(refresh()));
    // connect(SystemUpdate::instance(), SIGNAL(dbChanged(const QString&)),
    //         this, SLOT(refresh(const QString&)));
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QAbstractListModel(parent)
    , m_db(new UpdateDb(dbpath, this))
    , m_filter((int) UpdateDb::Filter::All)
    , m_updates()
{
    initialize();
}

void UpdateModel::initialize()
{
    connect(this, SIGNAL(filterChanged()), SLOT(clear()));
    connect(m_db, SIGNAL(changed()), this, SLOT(refresh()));
    connect(m_db, SIGNAL(changed(const QString)), this, SLOT(refresh(const QString)));

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

    QSharedPointer<Update> u = m_updates.at(row);

    switch (role) {
    case Qt::DisplayRole:
    case KindRole:
        return (int) u->kind();
    case IdRole:
        return u->identifier();
    case LocalVersionRole:
        return u->localVersion();
    case RemoteVersionRole:
        return u->remoteVersion();
    case RevisionRole:
        return u->revision();
    case InstalledRole:
        return u->installed();
    case CreatedAtRole:
        return u->createdAt();
    case UpdatedAtRole:
        return u->updatedAt();
    case TitleRole:
        return u->title();
    case DownloadHashRole:
        return u->downloadHash();
    case SizeRole:
        return u->binaryFilesize();
    case IconUrlRole:
        return u->iconUrl();
    case DownloadUrlRole:
        return u->downloadUrl();
    case CommandRole:
        return u->command();
    case ChangelogRole:
        return u->changelog();
    case TokenRole:
        return u->token();
    case UpdateStateRole:
        return (int) u->state();
    case ProgressRole:
        return u->progress();
    case AutomaticRole:
        return u->automatic();
    case DownloadIdRole:
        return u->downloadId();
    case ErrorRole:
        return u->error();
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

void UpdateModel::setFilter(const int &filter)
{
    if (filter != m_filter) {
        m_filter = filter;
        Q_EMIT (filterChanged());
    }
}

int UpdateModel::filter() const
{
    return m_filter;
}

void UpdateModel::clear()
{
    beginResetModel();
    m_updates.clear();
    endResetModel();

    refresh();
}

void UpdateModel::refresh(const QString &downloadId)
{
    QSharedPointer<Update> u = m_db->get(downloadId);
    int ix = UpdateModel::indexOf(m_updates, u);

    if (ix >= 0 && ix < m_updates.size()) {
        m_updates.replace(ix, u);
        emitRowChanged(ix);
    }
}

void UpdateModel::refresh()
{
    QList<QSharedPointer<Update> > now = m_db->updates((UpdateDb::Filter) m_filter);
    // QList<Update*> udb;
    // QList<Update*> uold;
    int oldCount = m_updates.size();

// for(item in old)
//     if (new does not contain item)
//         remove item from old

// for(item in new)
//     if (item exists in old)
//         if (position(item, old) == position(item, new))
//             continue // next loop iteration
//         else
//             move old item to position(item, new)
//     else
//         insert new item into old at position(item, new)

    // qWarning() << "m_updates..";
    for (int i = 0; i < m_updates.size(); i++) {
        QSharedPointer<Update> item = m_updates.at(i);
        // qWarning() << "m_updates:" << item->identifier();
        if (!UpdateModel::contains(now, item))
            removeRow(i);
    }

    // qWarning() << "now..";
    for (int i = 0; i < now.size(); i++) {
        QSharedPointer<Update> item = now.at(i);
        // qWarning() << "now:" << item->identifier() << i;

        // qWarning() << "\twhile m_updates...";
        for (int i = 0; i < m_updates.size(); i++) {
            QSharedPointer<Update> item = m_updates.at(i);
            // qWarning() << "\t\tm_updates:" << item->identifier();
        }

        int oldPos = UpdateModel::indexOf(m_updates, item);
        // qWarning() << "indeof" << item->identifier() << "was"<<oldPos;
        if (UpdateModel::contains(m_updates, item)) {
            // qWarning() << "m_updates contains" << item->identifier();
            if (oldPos == i) {
                if (!m_updates.at(oldPos)->deepEquals(item.data())) {
                    emitRowChanged(i);
                }
            } else {
                // qWarning() << "oldPos" << oldPos << "i" << i;
                moveRow(oldPos, i);
            }
        } else {
            insertRow(i, item);
        }
    }

    // // Added
    // for (int i = 0; i < fromdb.size(); i++) {
    //     QSharedPointer<Update> u = fromdb.at(i);
    //     if (!UpdateModel::contains(m_updates, u)) {
    //         insertRow(i, u);
    //     }
    // }

    // // Removed
    // for (int i = 0; i < m_updates.size(); i++) {
    //     QSharedPointer<Update> u = m_updates.at(i);
    //     if (!UpdateModel::contains(fromdb, u)) {
    //         removeRow(i);
    //     }
    // }

    // // Moved
    // for (int i = 0; i < tmp.size(); i++) {

    // }
    // for (int i = 0; i < fromdb.size(); i++) {
    //     QSharedPointer<Update> u = fromdb.at(i);
    //     if (UpdateModel::contains(tmp, u)) {
    //         int from = UpdateModel::indexOf(tmp, u);

    //         if (i == from)
    //             continue;

    //         qWarning() << "index of" << u->identifier() << from << i;
    //         // Move operation is on m_updates.
    //         // moveRow(from, i);
    //     }
    // }


    // Q_FOREACH(QSharedPointer<Update> u, m_updates) {
    //     uold.append(u.data());
    // }


    // QList<QSharedPointer<Update> > removed;
    //  Q_FOREACH(QSharedPointer<Update> u, m_updates) {
    //     if (!UpdateModel::contains(fromdb, u))
    //         removed.append(u);
    // }

    /* Ugly, but we need to be precise about changes so the view will not
    unnecessarily update too much of it. */
    // QList<QPair<int, int> > moveFromTo;
    // QList<int> added;
    // QList<int> changed;
    // for (int i3 = 0; i3 < udb.size(); i3++) {
    //     Update* u = udb.at(i3);
    //     if (uold.contains(u)) {
    //         int ixFrom = uold.indexOf(u);
    //         int ixTo = i3;

    //         if (ixFrom != ixTo) {
    //             // Moved
    //             moveFromTo << QPair<int, int>(ixFrom, ixTo);
    //         } else {
    //             // Changed?
    //             Update* uOld = uold.at(uold.indexOf(u));
    //             if (!u->deepEquals(uOld)) {
    //                 changed << i3;
    //             }
    //         }
    //     } else {
    //         // Added.
    //         added << i3;
    //     }
    // }

    // Q_FOREACH(int i, removed) {
    //     beginRemoveRows(QModelIndex(), i, i);
    //     endRemoveRows();
    // }

    // Q_FOREACH(int i, added) {
    //     beginInsertRows(QModelIndex(), i, i);
    //     endInsertRows();
    // }

    // qWarning() << "removing" << removed;
    // qWarning() << "moved" << moveFromTo;
    // qWarning() << "added" << added;
    // qWarning() << "changed" << changed;

    // m_updates = fromdb;

    // Q_FOREACH(int i, changed) {
    //     emitRowChanged(i);
    // }

    if (now.size() != oldCount) {
        Q_EMIT countChanged();
    }

}

void UpdateModel::insertRow(const int &row, const QSharedPointer<Update> &update)
{
    beginInsertRows(QModelIndex(), row, row);
    /* Append, then move to it's place. We move the displaced
    row to the end- */
    m_updates.append(update);
    m_updates.move(m_updates.size() - 1, row);
    endInsertRows();
}

void UpdateModel::removeRow(int row)
{
    if (0 <= row && row < m_updates.size()) {
        beginRemoveRows(QModelIndex(), row, row);
        m_updates.removeAt(row);
        endRemoveRows();
    }
}

void UpdateModel::moveRow(const int &from, const int &to)
{
    bool fromOk = (from >= 0 && from < m_updates.size());
    bool toOk = (to >= 0 && to < m_updates.size());
    int _to = to;
    if (fromOk && toOk && from != _to) {
        // If moving down
        if (_to > from) {
            _to++;
        }
        if (beginMoveRows(QModelIndex(), from, from, QModelIndex(), _to)) {
            // qWarning() << "move from" << from << "to" << _to;
            // qWarning() << "( really move from" << from << "to" << to << ")";
            // qWarning() << "move" << m_updates.at(from)->identifier()
            //            << "to" << to;
            m_updates.move(from, to);
            endMoveRows();
        } else {
            // qWarning() << "move from" << from << "to" << _to << "failed";
        }
    }
}

void UpdateModel::emitRowChanged(int row)
{
    if (0 <= row && row < m_updates.size()) {
        QModelIndex qmi = index(row, 0);
        Q_EMIT(dataChanged(qmi, qmi));
    }
}

// QSharedPointer<Update> UpdateModel::find(const QList<QSharedPointer<Update> > &list,
//                                          const QSharedPointer<Update> &update)
// {
//     Q_FOREACH(QSharedPointer<Update> u, list) {
//         if (*u == *update)
//             return u;
//     }
//     return QSharedPointer<Update>(nullptr);
// }

bool UpdateModel::contains(const QList<QSharedPointer<Update> > &list,
                           const QSharedPointer<Update> &update)
{
    return list.end() != std::find_if(
        list.begin(), list.end(),
        [update](const QSharedPointer<Update> other) {
            return *update == other.data();
        }
    );
}

int UpdateModel::indexOf(const QList<QSharedPointer<Update> > &list,
                          const QSharedPointer<Update> &update)
{
    for (int i = 0; i < list.size(); i++) {
        if (*list.at(i) == update.data()) {
            return i;
        }
    }
    return -1;
}
// void UpdateModel::refresh()
// {
//     qWarning() << "blast refresh...";
//     int oldCount = count();
//     if (!openDb())
//         qWarning() << "could not open db";

//     QSqlQuery q(m_db);
//     switch (m_filter) {
//     case UpdateDb::Filter::All:
//         // q = QSqlQuery(GET_ALL, m_db);
//         q.prepare(GET_ALL);
//         break;
//     case UpdateDb::Filter::Pending:
//     case UpdateDb::Filter::PendingClicksUpdates:
//         q.prepare(GET_PENDING_CLICKS);
//         q.bindValue(":installed", false);
//         q.bindValue(":kind", UpdateModel::KIND_CLICK);
//         break;
//     case UpdateDb::Filter::PendingSystemUpdates:
//         return; // We don't store these, so bail.
//     case UpdateDb::Filter::InstalledClicksUpdates:
//         q.prepare(GET_INSTALLED_CLICKS);
//         q.bindValue(":installed", true);
//         q.bindValue(":kind", UpdateModel::KIND_CLICK);
//         break;
//     case UpdateDb::Filter::InstalledSystemUpdates:
//         q.prepare(GET_INSTALLED_SYSUPDATES);
//         q.bindValue(":installed", true);
//         q.bindValue(":kind", UpdateModel::KIND_SYSTEM);
//         break;
//     case UpdateDb::Filter::Installed:
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
    m_db->setInstalled(downloadId);
}

void UpdateModel::startUpdate(const QString &downloadId)
{
    m_db->setStarted(downloadId);
}

void UpdateModel::processUpdate(const QString &downloadId)
{
    m_db->setProcessing(downloadId);
}

void UpdateModel::setError(const QString &downloadId, const QString &msg)
{
    m_db->setError(downloadId, msg);
}

// void UpdateModel::setState(const QString &downloadId,
//                            const SystemUpdate::UpdateState &state)
// {

// }

void UpdateModel::setProgress(const QString &downloadId,
                              const int &progress)
{
    m_db->setProgress(downloadId, progress);
}

void UpdateModel::setDownloadId(const QString &id, const int &revision,
                                const QString &downloadId)
{
    m_db->setDownloadId(id, revision, downloadId);
}

void UpdateModel::pauseUpdate(const QString &downloadId)
{
    m_db->setPaused(downloadId);
}

void UpdateModel::resumeUpdate(const QString &downloadId)
{
    m_db->setResumed(downloadId);
}

void UpdateModel::cancelUpdate(const QString &downloadId)
{
    m_db->setCanceled(downloadId);
}


// int UpdateModel::find(const QString &id, const int &revision) const
// {
//     int res = -1;
//     for (int i = 0; i < m_updates.size(); i++) {
//         if (m_updates.at(i)->identifier() == id
//             && m_updates.at(i)->revision() == revision) {
//             return i;
//         }
//     }
//     return res;
// }

// int UpdateModel::find(const QString &downloadId) const
// {
//     int res = -1;
//     for (int i = 0; i < m_updates.size(); i++) {
//         if (m_updates.at(i)->downloadId() == downloadId) {
//             return i;
//         }
//     }
//     return res;
// }

// bool UpdateModel::contains(const QString& downloadId) const
// {
//     for (int i = 0; i < m_updates.size(); i++) {
//         if (m_updates.at(i)->downloadId() == downloadId) {
//             return true;
//         }
//     }
//     return false;
// }
} // UpdatePlugin
