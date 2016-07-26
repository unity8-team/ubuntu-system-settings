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
    , m_db(new UpdateDb(this))
    , m_updates()
{
    initialize();
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QAbstractListModel(parent)
    , m_db(new UpdateDb(dbpath, this))
    , m_updates()
{
    initialize();
}

void UpdateModel::initialize()
{
    connect(m_db, SIGNAL(changed()), this, SLOT(refresh()));
    connect(m_db, SIGNAL(changed(const QSharedPointer<Update>&)),
            this, SLOT(refresh(const QSharedPointer<Update>&)));

    refresh();
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
        names[DownloadIdRole] = "downloadId";
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
    case DownloadIdRole:
        return u->downloadId();
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

// void UpdateModel::setFilter(const UpdateModel::Filter &filter)
// {
//     if (filter != m_filter) {
//         m_filter = filter;
//         Q_EMIT (filterChanged());
//     }
// }

// UpdateModel::Filter UpdateModel::filter() const
// {
//     return m_filter;
// }

void UpdateModel::clear()
{
    beginResetModel();
    m_updates.clear();
    endResetModel();

    refresh();
}

void UpdateModel::reset()
{
    m_db->reset();
    clear();
}

void UpdateModel::refresh(const QSharedPointer<Update> &update)
{
    int ix = UpdateModel::indexOf(m_updates, update);
    if (ix >= 0 && ix < m_updates.size()) {
        m_updates.replace(ix, update);
        emitRowChanged(ix);
    }
}

void UpdateModel::refresh()
{
    QList<QSharedPointer<Update> > now = m_db->updates();
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

QSharedPointer<Update> UpdateModel::get(const QString &id, const uint &revision)
{
    return find(id, revision);
}

QSharedPointer<Update> UpdateModel::get(const QString &id, const QString &version)
{
    return find(id, version);
}

QSharedPointer<Update> UpdateModel::find(const QString &id, const uint &revision)
{
    Q_FOREACH(QSharedPointer<Update> update, m_updates) {
        if (id == update->identifier() && revision == update->revision()) {
            return update;
        }
    }
    return QSharedPointer<Update>();
}

QSharedPointer<Update> UpdateModel::find(const QString &id, const QString &version)
{
    Q_FOREACH(QSharedPointer<Update> update, m_updates) {
        if (id == update->identifier() && version == update->remoteVersion()) {
            return update;
        }
    }
    return QSharedPointer<Update>();
}

QSharedPointer<Update> UpdateModel::fetch(const QString &id, const uint &revision)
{
    return m_db->get(id, revision);
}

void UpdateModel::add(const QSharedPointer<Update> &update)
{
    m_db->add(update);
}

void UpdateModel::update(const QSharedPointer<Update> &update)
{
    m_db->update(update);
}

void UpdateModel::remove(const QSharedPointer<Update> &update)
{
    m_db->remove(update);
}

bool UpdateModel::contains(const QString &id, const uint &revision) const
{
    Q_FOREACH(const QSharedPointer<Update> &update, m_updates) {
        if (id == update->identifier() && revision == update->revision()) {
            return true;
        }
    }
    return false;
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

void UpdateModel::setAvailable(const QString &id, const uint &revision,
                               const bool autoStart)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateAvailable);
        u->setInstalled(false);
        u->setProgress(0);
        u->setToken("");
        u->setDownloadId("");
        u->setError("");
        u->setAutomatic(autoStart);
        m_db->update(u);
    }
}

void UpdateModel::queueUpdate(const QString &id, const uint &revision,
                              const QString &downloadId)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateQueuedForDownload);
        u->setDownloadId(downloadId);
        m_db->update(u);
    }
}

void UpdateModel::setInstalled(const QString &id, const uint &revision)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setInstalled(true);
        u->setState(Update::State::StateInstallFinished);
        u->setUpdatedAt(QDateTime::currentDateTimeUtc());
        u->setDownloadId("");
        m_db->update(u);
    }
}

void UpdateModel::startUpdate(const QString &id, const uint &revision)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateDownloading);
        m_db->update(u);
    }
}

void UpdateModel::processUpdate(const QString &id, const uint &revision)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateInstalling);
        m_db->update(u);
    }
}

void UpdateModel::setError(const QString &id, const uint &revision,
                           const QString &msg)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateFailed);
        u->setError(msg);
        u->setDownloadId("");
        m_db->update(u);
    }
}

void UpdateModel::setDownloaded(const QString &id, const uint &revision)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateDownloaded);
        m_db->update(u);
    }
}

void UpdateModel::setProgress(const QString &id, const uint &revision,
                              const int &progress)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateDownloading);
        u->setProgress(progress);
        m_db->update(u);
    }
}

void UpdateModel::setInstalling(const QString &id, const uint &revision,
                                const int &progress)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateInstalling);
        u->setProgress(progress);
        m_db->update(u);
    }
}

void UpdateModel::pauseUpdate(const QString &id, const uint &revision)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateDownloadPaused);
        m_db->update(u);
    }
}

void UpdateModel::resumeUpdate(const QString &id, const uint &revision)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateDownloading);
        m_db->update(u);
    }
}

void UpdateModel::cancelUpdate(const QString &id, const uint &revision)
{
    QSharedPointer<Update> u = find(id, revision);
    if (!u.isNull()) {
        u->setState(Update::State::StateAvailable);
        u->setError("");
        u->setToken("");
        u->setDownloadId("");
        u->setProgress(0);
        m_db->update(u);
    }
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
    u->setTitle("Ubuntu");
    u->setRevision(revision);
    u->setBinaryFilesize((int) updateSize);
    u->setRemoteVersion(version);
    u->setState(Update::State::StateAvailable);
    u->setIconUrl(QLatin1String(
        "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
    ));

    m_db->add(u);
}

UpdateModelFilter::UpdateModelFilter(UpdateModel *model, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(model);
}

uint UpdateModelFilter::kindFilter() const
{
    return (uint) m_kind;
}

void UpdateModelFilter::filterOnKind(const uint &kind)
{
    Update::Kind _kind = (Update::Kind) kind;
    if (_kind != m_kind) {
        m_kind = _kind;
        Q_EMIT kindFilterChanged();
    }
    m_kindEnabled = true;
    invalidate();
}

bool UpdateModelFilter::installed() const
{
    return m_installed;
}

void UpdateModelFilter::filterOnInstalled(const bool installed)
{
    if (installed != m_installed) {
        m_installed = installed;
        Q_EMIT installedChanged();
    }

    m_installedEnabled = true;
    invalidateFilter();

    if (installed) {
        // Installed updates are sorted date (most recent first).
        setSortRole(UpdateModel::UpdatedAtRole);
        sort(0, Qt::DescendingOrder);
    } else {
        // Pending updates are sorted by title, a-z.
        setSortRole(UpdateModel::TitleRole);
        sort(0, Qt::AscendingOrder);
    }
}

bool UpdateModelFilter::filterAcceptsRow(int sourceRow,
                                         const QModelIndex &sourceParent) const
{
    bool accepts = true;
    QModelIndex childIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    if (accepts && m_installedEnabled) {
        const bool installed = childIndex.model()->data(
            childIndex, UpdateModel::InstalledRole
        ).toBool();
        accepts = installed == m_installed;
    }

    if (accepts && m_kindEnabled) {
        const Update::Kind kind = (Update::Kind) childIndex.model()->data(
            childIndex, UpdateModel::KindRole
        ).toUInt();
        accepts = m_kind == kind;
    }

    return accepts;
}
} // UpdatePlugin
