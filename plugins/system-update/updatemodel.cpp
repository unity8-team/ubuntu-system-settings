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

#include "updatemodel.h"
#include <QDebug>

namespace UpdatePlugin
{
UpdateModel::UpdateModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_db(new UpdateDb(this))
{
    initialize();
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QAbstractListModel(parent)
    , m_db(new UpdateDb(dbpath, this))
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
        names[SignedDownloadUrl] = "signedDownloadUrl";
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

    auto update = m_updates.at(row);

    switch (role) {
    case Qt::DisplayRole:
    case KindRole:
        return (uint) update->kind();
    case IdRole:
        return update->identifier();
    case LocalVersionRole:
        return update->localVersion();
    case RemoteVersionRole:
        return update->remoteVersion();
    case RevisionRole:
        return update->revision();
    case InstalledRole:
        return update->installed();
    case CreatedAtRole:
        return update->createdAt();
    case UpdatedAtRole:
        return update->updatedAt();
    case TitleRole:
        return update->title();
    case DownloadHashRole:
        return update->downloadHash();
    case DownloadIdRole:
        return update->downloadId();
    case SizeRole:
        return update->binaryFilesize();
    case IconUrlRole:
        return update->iconUrl();
    case DownloadUrlRole:
        return update->downloadUrl();
    case CommandRole:
        return update->command();
    case ChangelogRole:
        return update->changelog();
    case TokenRole:
        return update->token();
    case UpdateStateRole:
        return (uint) update->state();
    case ProgressRole:
        return update->progress();
    case AutomaticRole:
        return update->automatic();
    case ErrorRole:
        return update->error();
    case PackageNameRole:
        return update->packageName();
    case SignedDownloadUrl:
        return update->signedDownloadUrl();
    }
    return QVariant();
}

int UpdateModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_updates.size();
}

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
    UpdateList now = m_db->updates();
    int oldCount = m_updates.size();

    // qWarning() << "m_updates is";
    for (int i = 0; i < m_updates.size(); i++) {
        auto item = m_updates.at(i);
        // qWarning() << "\t" << i << item->identifier();
        if (!UpdateModel::contains(now, item)) {
            // qWarning() << "\t\t removing" << i;
            removeRow(i);
        }
    }

    // qWarning() << "now is";
    for (int i = 0; i < now.size(); i++) {
        auto item = now.at(i);
        // qWarning() << "\t" << item->identifier() << i;

        // qWarning() << "\t\twhile m_updates...";
        for (int i = 0; i < m_updates.size(); i++) {
            auto item = m_updates.at(i);
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
        auto item = m_updates.at(i);
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

int UpdateModel::indexOf(const UpdateList &list,
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

QSharedPointer<Update> UpdateModel::get(const QSharedPointer<Update> &other)
{
    return find(other->identifier(), other->revision());
}

QSharedPointer<Update> UpdateModel::find(const QString &id, const uint &revision)
{
    Q_FOREACH(auto update, m_updates) {
        if (id == update->identifier() && revision == update->revision()) {
            return update;
        }
    }
    return QSharedPointer<Update>(nullptr);
}

QSharedPointer<Update> UpdateModel::find(const QString &id, const QString &version)
{
    Q_FOREACH(auto update, m_updates) {
        if (id == update->identifier() && version == update->remoteVersion()) {
            return update;
        }
    }
    return QSharedPointer<Update>(nullptr);
}

QSharedPointer<Update> UpdateModel::fetch(const QString &id, const uint &revision)
{
    return m_db->get(id, revision);
}

QSharedPointer<Update> UpdateModel::fetch(const QSharedPointer<Update> &other)
{
    return m_db->get(other->identifier(), other->revision());
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
    Q_FOREACH(const auto &update, m_updates) {
        if (id == update->identifier() && revision == update->revision()) {
            return true;
        }
    }
    return false;
}

bool UpdateModel::contains(const UpdateList &list,
                           const QSharedPointer<Update> &update)
{
    return list.end() != std::find_if(
        list.begin(), list.end(),
        [update](const QSharedPointer<Update> other) {
            return *update == *other.data();
        }
    );
}

void UpdateModel::setAvailable(const QString &id, const uint &rev,
                               const bool autoStart)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        update->setState(Update::State::StateAvailable);
        update->setInstalled(false);
        update->setProgress(0);
        update->setToken("");
        update->setDownloadId("");
        update->setAutomatic(autoStart);
        m_db->update(update);
    }
}

void UpdateModel::queueUpdate(const QString &id, const uint &rev,
                              const QString &downloadId)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        update->setState(Update::State::StateQueuedForDownload);
        update->setDownloadId(downloadId);
        m_db->update(update);
    }
}

void UpdateModel::setInstalled(const QString &id, const uint &rev)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setInstalled(true);
        update->setState(Update::State::StateInstallFinished);
        update->setUpdatedAt(QDateTime::currentDateTimeUtc());
        update->setDownloadId("");
        update->setError("");
        m_db->update(update);
    }
}

void UpdateModel::startUpdate(const QString &id, const uint &rev,
                              const bool automatic)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        Update::State state = automatic ?
                              Update::State::StateDownloadingAutomatically :
                              Update::State::StateDownloading;
        update->setState(state);
        m_db->update(update);
    }
}

void UpdateModel::processUpdate(const QString &id, const uint &rev)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        update->setState(Update::State::StateInstalling);
        m_db->update(update);
    }
}

void UpdateModel::setError(const QString &id, const uint &rev,
                           const QString &msg)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setState(Update::State::StateFailed);
        update->setError(msg);
        update->setDownloadId("");
        m_db->update(update);
    }
}

void UpdateModel::setDownloaded(const QString &id, const uint &rev)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        update->setState(Update::State::StateDownloaded);
        m_db->update(update);
    }
}

void UpdateModel::setProgress(const QString &id, const uint &rev,
                              const int &progress)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        update->setState(Update::State::StateDownloading);
        update->setProgress(progress);
        m_db->update(update);
    }
}

void UpdateModel::setInstalling(const QString &id, const uint &rev,
                                const int &progress)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        update->setState(Update::State::StateInstalling);
        update->setProgress(progress);
        m_db->update(update);
    }
}

void UpdateModel::pauseUpdate(const QString &id, const uint &rev,
                              const bool automatic)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        auto state = automatic ?
                     Update::State::StateAutomaticDownloadPaused :
                     Update::State::StateDownloadPaused;
        update->setState(state);
        m_db->update(update);
    }
}

void UpdateModel::resumeUpdate(const QString &id, const uint &rev,
                               const bool automatic)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setError("");
        auto state = automatic ?
                     Update::State::StateDownloadingAutomatically :
                     Update::State::StateDownloading;
        update->setState(state);
        m_db->update(update);
    }
}

void UpdateModel::cancelUpdate(const QString &id, const uint &rev)
{
    auto update = find(id, rev);
    if (!update.isNull()) {
        update->setState(Update::State::StateAvailable);
        update->setError("");
        update->setDownloadId("");
        update->setProgress(0);
        m_db->update(update);
    }
}

void UpdateModel::setImageUpdate(const QString &id, const int &version,
                                 const int &updateSize)
{
    auto update = QSharedPointer<Update>(new Update);
    update->setIdentifier(id);
    update->setKind(Update::Kind::KindImage);
    update->setProgress(0);
    update->setTitle("Ubuntu");
    update->setRevision((uint) version);
    update->setBinaryFilesize((int) updateSize);
    update->setRemoteVersion(QString::number(version));
    update->setState(Update::State::StateAvailable);
    update->setIconUrl(QLatin1String(
        "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
    ));

    m_db->add(update);
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
