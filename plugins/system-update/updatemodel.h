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

#include <QAbstractListModel>
#include <QModelIndex>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlRecord>

#include "updatestore.h"

namespace UpdatePlugin
{

struct UpdateStruct {
    QString kind;
    QString id;
    QString localVersion;
    QString remoteVersion;
    int revision;
    QString state;
    QDateTime createdAt;
    QDateTime updatedAt;
    QString title;
    QString downloadHash;
    int size;
    QString iconUrl;
    QString downloadUrl;
    QStringList command;
    QString changelog;
    QString token;
    SystemUpdate::UpdateState updateState;
    int progress;
    bool automatic;

    bool operator==(const UpdateStruct &other) const
    {
       if (other.id == id && other.revision == revision)
          return true;
       else
          return false;
    }
};

class UpdateModel : public QAbstractListModel
{
    Q_OBJECT
    // TODO: use flags
    Q_PROPERTY(UpdateTypes filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_ENUMS(UpdateTypes)
public:
    enum class UpdateTypes
    {
        All,
        Pending,
        PendingClicksUpdates,
        PendingSystemUpdates,
        InstalledClicksUpdates,
        InstalledSystemUpdates,
        Installed
    };

    enum Roles
    {
      // Qt::DisplayRole holds the title of the app
      KindRole = Qt::UserRole,
      IconUrlRole,
      IdRole,
      LocalVersionRole,
      RemoteVersionRole,
      RevisionRole,
      StateRole,
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
      LastRole = AutomaticRole
    };

    explicit UpdateModel(QObject *parent = 0);
    ~UpdateModel();

    // For testing.
    explicit UpdateModel(const QString &dbpath, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    int count() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void setFilter(const UpdateTypes &filter);
    UpdateTypes filter() const;

public slots:
    Q_INVOKABLE void update();
    Q_INVOKABLE void updateItem(const QString &id, const int &revision);

signals:
    void countChanged();
    void filterChanged();

private:
    void initialize();
    int find(const QString &id, const int &revision) const;
    void setValues(UpdateStruct *update, QSqlQuery *query);
    //const static char* COLUMN_NAMES[];
    UpdateTypes m_filter;
    UpdateStore *m_store;
    QList<UpdateStruct> m_updates;
};
} // UpdatePlugin

#endif // UPDATE_MODEL_H
