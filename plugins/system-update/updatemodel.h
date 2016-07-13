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
#include <QDateTime>
#include <QModelIndex>
#include <QSqlDatabase>
#include <QSortFilterProxyModel>

namespace UpdatePlugin
{
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
      SizeRole,
      DownloadUrlRole,
      CommandRole,
      ChangelogRole,
      TokenRole,
      UpdateStateRole,
      ProgressRole,
      AutomaticRole,
      DownloadIdRole,
      ErrorRole,
      PackageNameRole,
      LastRole = PackageNameRole
    };

    explicit UpdateModel(QObject *parent = 0);
    ~UpdateModel() {};

    // For testing, when we want to explicitly set the database path.
    explicit UpdateModel(const QString &dbpath, QObject *parent = 0);

    UpdateDb* db();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void add(const QSharedPointer<Update> &update);
    void update(const QSharedPointer<Update> &update);
    void remove(const QSharedPointer<Update> &update);
    QSharedPointer<Update> fetch(const QString &id, const uint &revision);
    QSharedPointer<Update> get(const QString &id, const uint &revision);

    Q_INVOKABLE void setAvailable(const QString &id, const uint &revision);
    Q_INVOKABLE void setInstalled(const QString &id, const uint &revision);
    Q_INVOKABLE void setError(const QString &id, const uint &revision, const QString &msg);
    Q_INVOKABLE void setProgress(const QString &id, const uint &revision,
                     const int &progress);
    Q_INVOKABLE void setDownloaded(const QString &id, const uint &revision);
    Q_INVOKABLE void startUpdate(const QString &id, const uint &revision);
    Q_INVOKABLE void queueUpdate(const QString &id, const uint &revision);
    Q_INVOKABLE void processUpdate(const QString &id, const uint &revision);
    Q_INVOKABLE void pauseUpdate(const QString &id, const uint &revision);
    Q_INVOKABLE void resumeUpdate(const QString &id, const uint &revision);
    Q_INVOKABLE void cancelUpdate(const QString &id, const uint &revision);
    Q_INVOKABLE void setImageUpdate(const QString &id, const QString &version,
                                    const int &updateSize);

public slots:
    void refresh();
    void refresh(const QSharedPointer<Update> &update);
    void clear();

signals:
    void countChanged();

private:
    void insertRow(const int &row, const QSharedPointer<Update> &update);
    void removeRow(int i);
    void moveRow(const int &from, const int &to);
    void emitRowChanged(int row);
    void initialize();
    bool contains(const QString &id, const uint &revision) const;
    static bool contains(const QList<QSharedPointer<Update> > &list,
                         const QSharedPointer<Update> &update);
    QSharedPointer<Update> find(const QString &id, const uint &revision);
    static int indexOf(const QList<QSharedPointer<Update> > &list,
                        const QSharedPointer<Update> &update);
    UpdateDb* m_db;
    QList<QSharedPointer<Update> > m_updates;
};

class UpdateModelFilter: public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(Update::Kinds kinds READ kinds WRITE filterOnKinds
               NOTIFY kindsChanged)
    Q_PROPERTY(bool installed READ installed WRITE filterOnInstalled
               NOTIFY installedChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(int sortBy READ sortRole WRITE setSortRole NOTIFY sortRoleChanged)

public:
    explicit UpdateModelFilter(QObject *parent = 0);

    // For testing, when we want to explicitly set the database path.
    explicit UpdateModelFilter(UpdateModel *model, QObject *parent = 0);

    Update::Kinds kinds() const;
    void filterOnKinds(const Update::Kinds &kinds);

    bool installed() const;
    void filterOnInstalled(const bool installed);

signals:
    void kindsChanged();
    void installedChanged();
    void countChanged();
    void sortRoleChanged();

protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;
//    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;

private:
    Update::Kinds m_kinds = Update::Kind::KindUnknown;
    bool m_kindEnabled = false;
    bool m_installed = false;
    bool m_installedEnabled = false;
};

} // UpdatePlugin

#endif // UPDATE_MODEL_H
