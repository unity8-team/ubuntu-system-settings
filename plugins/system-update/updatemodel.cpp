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
    , m_filter(UpdateModel::Filter::All)
    , m_updates()
{
    initialize();
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QAbstractListModel(parent)
    , m_db(new UpdateDb(dbpath, this))
    , m_filter(UpdateModel::Filter::All)
    , m_updates()
{
    initialize();
}

void UpdateModel::initialize()
{
    connect(this, SIGNAL(filterChanged()), SLOT(clear()));
    connect(m_db, SIGNAL(changed()), this, SLOT(refresh()));
    connect(m_db, SIGNAL(changed(const QString, const uint)),
            this, SLOT(refresh(const QString, const uint)));
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
        names[ErrorRole] = "error";
        names[PackageNameRole] = "packageName";
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
        return (uint) u->kind();
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
        return (uint) u->state();
    case ProgressRole:
        return u->progress();
    case AutomaticRole:
        return u->automatic();
    case ErrorRole:
        return u->error();
    case PackageNameRole:
        return u->packageName();
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

void UpdateModel::setFilter(const UpdateModel::Filter &filter)
{
    if (filter != m_filter) {
        m_filter = filter;
        Q_EMIT (filterChanged());
    }
}

UpdateModel::Filter UpdateModel::filter() const
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

void UpdateModel::refresh(const QString &id, const uint &revision)
{
    QSharedPointer<Update> u = m_db->get(id, revision);
    int ix = UpdateModel::indexOf(m_updates, u);

    if (ix >= 0 && ix < m_updates.size()) {
        m_updates.replace(ix, u);
        emitRowChanged(ix);
    }
}

void UpdateModel::refresh()
{
    QList<QSharedPointer<Update> > now = m_db->updates((uint) m_filter);
    int oldCount = m_updates.size();

    // qWarning() << "m_updates is";
    for (int i = 0; i < m_updates.size(); i++) {
        QSharedPointer<Update> item = m_updates.at(i);
        // qWarning() << "\t" << i << item->identifier();
        if (!UpdateModel::contains(now, item)) {
            // qWarning() << "\t\t removing" << i;
            removeRow(i);
        }
    }

    // qWarning() << "now is";
    for (int i = 0; i < now.size(); i++) {
        QSharedPointer<Update> item = now.at(i);
        // qWarning() << "\t" << item->identifier() << i;

        // qWarning() << "\t\twhile m_updates...";
        for (int i = 0; i < m_updates.size(); i++) {
            QSharedPointer<Update> item = m_updates.at(i);
            // qWarning() << "\t\t\tm_updates:" << item->identifier();
        }

        int oldPos = UpdateModel::indexOf(m_updates, item);
        // qWarning() << "\t\tindexOf" << item->identifier() << "was"<<oldPos;
        if (UpdateModel::contains(m_updates, item)) {
            // qWarning() << "\t\tm_updates contains" << item->identifier();
            if (oldPos == i) {
                if (!m_updates.at(oldPos)->deepEquals(item.data())) {
                    // qWarning() << "\t\t\t row"<<i<<"changed";
                    emitRowChanged(i);
                }
            } else {
                // qWarning() << "\t\t\t moving"<<i<<"from"<<oldPos;
                moveRow(oldPos, i);
            }
        } else {
            // qWarning() << "\t\t\t inserting"<<i<<item->identifier();
            insertRow(i, item);
        }
    }

    // qWarning() << "\n\nm_updates after operation";
    for (int i = 0; i < m_updates.size(); i++) {
        QSharedPointer<Update> item = m_updates.at(i);
        // qWarning() << "\tm_updates:" << item->identifier();
    }

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
    // qWarning() << "moving rows" << from << to;
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
            //             << "to" << to;
            m_updates.move(from, to);
            endMoveRows();
        } else {
            qWarning() << "move from" << from << "to" << _to << "failed";
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

bool UpdateModel::contains(const QList<QSharedPointer<Update> > &list,
                           const QSharedPointer<Update> &update)
{
    return list.end() != std::find_if(
        list.begin(), list.end(),
        [update](const QSharedPointer<Update> other) {
            return *update == *other.data();
        }
    );
}

int UpdateModel::indexOf(const QList<QSharedPointer<Update> > &list,
                          const QSharedPointer<Update> &update)
{
    for (int i = 0; i < list.size(); i++) {
        if (*list.at(i) == *update.data()) {
            return i;
        }
    }
    return -1;
}
void UpdateModel::queueUpdate(const QString &id, const uint &revision)
{
    m_db->setQueued(id, revision);
}

void UpdateModel::setInstalled(const QString &id, const uint &revision)
{
    m_db->setInstalled(id, revision);
}

void UpdateModel::startUpdate(const QString &id, const uint &revision)
{
    m_db->setStarted(id, revision);
}

void UpdateModel::processUpdate(const QString &id, const uint &revision)
{
    m_db->setProcessing(id, revision);
}

void UpdateModel::setError(const QString &id, const uint &revision, const QString &msg)
{
    m_db->setError(id, revision, msg);
}

void UpdateModel::setDownloaded(const QString &id, const uint &revision)
{
    m_db->setDownloaded(id, revision);
}

void UpdateModel::setProgress(const QString &id, const uint &revision,
                              const int &progress)
{
    m_db->setProgress(id, revision, progress);
}

void UpdateModel::pauseUpdate(const QString &id, const uint &revision)
{
    m_db->setPaused(id, revision);
}

void UpdateModel::resumeUpdate(const QString &id, const uint &revision)
{
    m_db->setResumed(id, revision);
}

void UpdateModel::cancelUpdate(const QString &id, const uint &revision)
{
    m_db->setCanceled(id, revision);
}

void UpdateModel::setImageUpdate(const QString &id, const QString &version,
                                 const int &updateSize)
{
    QSharedPointer<Update> u = QSharedPointer<Update>(new Update);
    u->setIdentifier(id);
    u->setKind(Update::Kind::KindImage);

    bool ok = false;
    uint revision = version.toUInt(&ok, 10);
    if (!ok) {
        qWarning() << "Did not understand image version" << version;
        revision = 0;
    }
    u->setProgress(0);
    u->setRevision(revision);
    u->setBinaryFilesize((int) updateSize);
    u->setRemoteVersion(version);
    u->setState(Update::State::StateAvailable);

    m_db->add(u);
}
} // UpdatePlugin
