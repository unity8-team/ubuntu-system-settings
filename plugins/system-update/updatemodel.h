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

namespace UpdatePlugin
{
class UpdateModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Filter filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_ENUMS(Filter)
public:
    enum class Filter : uint
    {
        All = 0,
        Pending,
        PendingClicks,
        PendingImage,
        InstalledClicks,
        InstalledImage,
        Installed
    };

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
      LastRole = ErrorRole
    };

    explicit UpdateModel(QObject *parent = 0);
    ~UpdateModel() {};

    // For testing.
    explicit UpdateModel(const QString &dbpath, QObject *parent = 0);

    UpdateDb* db();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int count() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    Filter filter() const;
    void setFilter(const Filter &filter);

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
                                    const int &updateSize, const int &downloadMode);
    // Q_INVOKABLE void setDownloadId(const QString &id, const uint &revision,
    //                    const QString &downloadId);
    // Q_INVOKABLE bool contains(const QString &downloadId) const;

public slots:
    void refresh();
    void refresh(const QString &id, const uint &revision);
    void clear();

signals:
    void countChanged();
    void filterChanged();
    // void changed();
    // void updateChanged(const QString &id, const uint &revision);
    // void updateChanged(const QString &downloadId);

private:
    void insertRow(const int &row, const QSharedPointer<Update> &update);
    void removeRow(int i);
    void moveRow(const int &from, const int &to);
    void emitRowChanged(int row);
    void initialize();
    // int find(const QString &id, const uint &revision) const;
    // int find(const QString &downloadId) const;
    // static QSharedPointer<Update> find(const QList<QSharedPointer<Update> > &list,
    //                                    const QSharedPointer<Update> &update);
    static bool contains(const QList<QSharedPointer<Update> > &list,
                         const QSharedPointer<Update> &update);
    static int indexOf(const QList<QSharedPointer<Update> > &list,
                        const QSharedPointer<Update> &update);
    UpdateDb* m_db;
    Filter m_filter;
    QList<QSharedPointer<Update> > m_updates;
};
} // UpdatePlugin

#endif // UPDATE_MODEL_H
