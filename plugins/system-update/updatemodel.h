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

#ifndef UPDATE_MODEL_H
#define UPDATE_MODEL_H

#include "update.h"
#include "updatedb.h"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QSortFilterProxyModel>

namespace UpdatePlugin
{
typedef QList<QSharedPointer<Update>> UpdateList;

class UpdateModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles
    {
      KindRole = Qt::UserRole,
      IconUrlRole,
      IdRole,
      LocalVersionRole,
      RemoteVersionRole,
      RevisionRole,
      InstalledRole,
      CreatedAtRole,
      UpdatedAtRole,
      TitleRole,
      DownloadHashRole,
      DownloadIdRole,
      SizeRole,
      DownloadUrlRole,
      CommandRole,
      ChangelogRole,
      TokenRole,
      UpdateStateRole,
      ProgressRole,
      AutomaticRole,
      ErrorRole,
      PackageNameRole,
      SignedDownloadUrlRole,
      LastRole = SignedDownloadUrlRole
    };

    explicit UpdateModel(QObject *parent = nullptr);
    ~UpdateModel() {};

    // For testing, when we want to explicitly set the database path.
    explicit UpdateModel(const QString &dbpath, QObject *parent = nullptr);
    UpdateDb* db();
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    void add(const QSharedPointer<Update> &update);

    /* Update an Update.
     *
     * Update information about an update. If the Update does not exist, it
     * will be created.
     */
    void update(const QSharedPointer<Update> &update);
    void remove(const QSharedPointer<Update> &update);
    void remove(const QString &id, const uint &revision);

    // Fetch will grab data from the db, while get returns a from cache.
    QSharedPointer<Update> fetch(const QString &id, const uint &revision);
    QSharedPointer<Update> fetch(const QSharedPointer<Update> &other);
    QSharedPointer<Update> get(const QString &id, const uint &revision);
    QSharedPointer<Update> get(const QString &id, const QString &version);
    QSharedPointer<Update> get(const QSharedPointer<Update> &other);

    Q_INVOKABLE void setAvailable(const QString &id, const uint &rev,
                                  const bool autoStart = false);
    Q_INVOKABLE void setInstalled(const QString &id, const uint &rev);
    Q_INVOKABLE void setError(const QString &id, const uint &rev,
                              const QString &msg);
    Q_INVOKABLE void setProgress(const QString &id, const uint &rev,
                     const int &progress);
    Q_INVOKABLE void setDownloaded(const QString &id, const uint &rev);
    Q_INVOKABLE void setInstalling(const QString &id, const uint &rev,
                                   const int &progress = 0);
    Q_INVOKABLE void startUpdate(const QString &id, const uint &rev,
                                 const bool automatic = false);
    Q_INVOKABLE void queueUpdate(const QString &id, const uint &rev,
                                 const QString &downloadId);
    Q_INVOKABLE void processUpdate(const QString &id, const uint &rev);
    Q_INVOKABLE void pauseUpdate(const QString &id, const uint &rev,
                                 const bool automatic = false);
    Q_INVOKABLE void resumeUpdate(const QString &id, const uint &rev,
                                  const bool automatic = false);
    Q_INVOKABLE void cancelUpdate(const QString &id, const uint &rev);
    Q_INVOKABLE void setImageUpdate(const QString &id, const int &version,
                                    const int &updateSize);
    static bool contains(const UpdateList &list,
                         const QSharedPointer<Update> &update);
public Q_SLOTS:
    void refresh();
    void refresh(const QSharedPointer<Update> &update);
    void clear();
    void reset();
Q_SIGNALS:
    void countChanged();
private:
    void insertRow(const int &row, const QSharedPointer<Update> &update);
    void removeRow(int i);
    void moveRow(const int &from, const int &to);
    void emitRowChanged(int row);
    void initialize();
    bool contains(const QString &id, const uint &revision) const;
    QSharedPointer<Update> find(const QString &id, const uint &revision);
    QSharedPointer<Update> find(const QString &id, const QString &version);
    static int indexOf(const UpdateList &list,
                       const QSharedPointer<Update> &update);
    UpdateDb* m_db;
    UpdateList m_updates;
};

class UpdateModelFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit UpdateModelFilter(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent) {};

    /* Return the Kind filter currently installed.
     *
     * Note that this filter is disabled by default.
     */
    uint kindFilter() const;
    void filterOnKind(const uint &kind);

    /* Return the install filter.
     *
     * true means installed, false means not installed (pending).
     * Note that this filter is disabled by default.
     */
    bool installed() const;
    void filterOnInstalled(const bool installed);
Q_SIGNALS:
    void kindFilterChanged();
    void installedChanged();
    void countChanged();
    void sortRoleChanged();
protected:
    virtual bool filterAcceptsRow(
        int sourceRow, const QModelIndex &sourceParent
    ) const override;
private:
    Update::Kind m_kind = Update::Kind::KindUnknown;
    bool m_kindEnabled = false;
    bool m_installed = false;
    bool m_installedEnabled = false;
};
} // UpdatePlugin

#endif // UPDATE_MODEL_H
