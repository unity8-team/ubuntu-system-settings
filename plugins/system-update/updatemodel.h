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
#include "updatestruct.h"

#include <QAbstractListModel>
#include <QDateTime>
#include <QModelIndex>
#include <QSqlDatabase>

namespace UpdatePlugin
{
class UpdateModel : public QAbstractListModel
{
    Q_OBJECT
    // TODO: use flags
    Q_PROPERTY(Update::Filter filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles
    {
      // Qt::DisplayRole holds the title of the app
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
      LastRole = ErrorRole
    };

    explicit UpdateModel(QObject *parent = 0);
    ~UpdateModel() {};

    // For testing.
    explicit UpdateModel(const QString &dbpath, QObject *parent = 0);

    UpdateDb* db();
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    int count() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Update::Filter filter() const;
    void setFilter(const Update::Filter &filter);

    Q_INVOKABLE void setInstalled(const QString &downloadId);
    Q_INVOKABLE void setError(const QString &downloadId, const QString &msg);
    Q_INVOKABLE void setProgress(const QString &downloadId,
                     const int &progress);
    Q_INVOKABLE void startUpdate(const QString &downloadId);
    Q_INVOKABLE void processUpdate(const QString &downloadId);
    Q_INVOKABLE void pauseUpdate(const QString &downloadId);
    Q_INVOKABLE void resumeUpdate(const QString &downloadId);
    Q_INVOKABLE void cancelUpdate(const QString &downloadId);
    Q_INVOKABLE void setDownloadId(const QString &id, const int &revision,
                       const QString &downloadId);
    Q_INVOKABLE bool contains(const QString &downloadId) const;

signals:
    void countChanged();
    void filterChanged();
    // void changed();
    // void updateChanged(const QString &id, const int &revision);
    // void updateChanged(const QString &downloadId);

private:
    void initialize();
    int find(const QString &id, const int &revision) const;
    int find(const QString &downloadId) const;

    UpdateDb* m_db;
    Update::Filter m_filter;
    QList<UpdateStruct> m_updates;
};
} // UpdatePlugin

#endif // UPDATE_MODEL_H
