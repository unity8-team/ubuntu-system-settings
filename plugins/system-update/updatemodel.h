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
#include <QDateTime>
#include <QModelIndex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "systemupdate.h"
#include "clickupdatemetadata.h"
// #include "updatestore.h"

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
    QString downloadId;
    QString error;

    bool operator==(const UpdateStruct &other) const
    {
        if (other.id == id && other.revision == revision)
            return true;
        else if (other.downloadId == downloadId)
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
      DownloadIdRole,
      ErrorRole,
      LastRole = ErrorRole
    };

    explicit UpdateModel(QObject *parent = 0);
    ~UpdateModel();

    // For testing.
    explicit UpdateModel(const QString &dbpath, QObject *parent = 0);

    static const QString KIND_CLICK;
    static const QString KIND_SYSTEM;
    static const QString STATE_PENDING;
    static const QString STATE_INSTALLED;

    QSqlDatabase db() const; // For testing.
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    int count() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    UpdateTypes filter() const;
    void setFilter(const UpdateTypes &filter);

    QDateTime lastCheckDate();
    void setLastCheckDate(const QDateTime &lastCheckUtc);

    void add(const ClickUpdateMetadata *meta);
    ClickUpdateMetadata* getPending(const QString &id);

    void add(const QString &kind, const QString &id,
             const int &revision, const QString &version,
             const QString &changelog, const QString &title,
             const QString &iconUrl, const int &size,
             const bool automatic);

    void setInstalled(const QString &downloadId);
    void setError(const QString &downloadId, const QString &msg);
    void setProgress(const QString &downloadId,
                     const int &progress);
    void pauseUpdate(const QString &downloadId);
    void resumeUpdate(const QString &downloadId);
    void cancelUpdate(const QString &downloadId);
    void setDownloadId(const QString &id, const int &revision,
                       const QString &downloadId);

    Q_INVOKABLE bool contains(const QString &downloadId) const;

public slots:
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void refreshItem(const QString &id, const int &revision);

signals:
    void countChanged();
    void filterChanged();
    void changed();
    void updateChanged(const QString &id, const int &revision);

private:
    void initialize();
    bool createDb();
    void initializeDb();
    void pruneDb();
    bool openDb();

    int find(const QString &id, const int &revision) const;
    void setValues(UpdateStruct *update, QSqlQuery *query);

    void setState(const QString &downloadId,
                  const SystemUpdate::UpdateState &state);
    void unsetDownloadId(const QString &downloadId);
    QString updateStateToString(const SystemUpdate::UpdateState &state);
    SystemUpdate::UpdateState stringToUpdateState(const QString &state);

    QSqlDatabase m_db;
    QString m_dbpath;
    QString m_connectionName;

    UpdateTypes m_filter;
    QList<UpdateStruct> m_updates;
};
} // UpdatePlugin

#endif // UPDATE_MODEL_H
