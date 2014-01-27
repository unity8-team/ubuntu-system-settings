/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Diego Sarmentero <diego.sarmentero@canonical.com>
 *
*/

#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QtQml>
#include <QHash>
#include <QList>
#include <QVariant>
#include <QVariantList>
#include "system_update.h"
#include "update.h"

namespace UpdatePlugin {

class UpdateManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList model READ model NOTIFY modelChanged)
    Q_PROPERTY(int downloadMode READ downloadMode WRITE setDownloadMode
               NOTIFY downloadModeChanged)

Q_SIGNALS:
    void modelChanged();
    void updatesNotFound();
    void updateAvailableFound();
    void errorFound();
    void downloadModeChanged();
    void systemUpdateDownloaded();
    void updateProcessFailed(QString message);
    void systemUpdateFailed();
    
public:
    explicit UpdateManager(QObject *parent = 0);
    ~UpdateManager();

    Q_INVOKABLE void checkUpdates();
    Q_INVOKABLE void startDownload(const QString &packagename);
    Q_INVOKABLE void pauseDownload(const QString &packagename);
    Q_INVOKABLE void retryDownload(const QString &packagename);
    Q_INVOKABLE void applySystemUpdate() { m_systemUpdate.applyUpdate(); }

    QVariantList model() const { return m_model; }
    int downloadMode() { return m_systemUpdate.downloadMode(); }
    void setDownloadMode(int mode) { m_systemUpdate.setDownloadMode(mode); }

#ifdef TESTS
    // For testing purposes
    QHash<QString, Update*> get_apps() { return m_apps; }
    QVariantList get_model() { return m_model; }
    int get_downloadMode() { return m_downloadMode; }
#endif

public Q_SLOTS:
    void registerSystemUpdate(const QString& packageName, Update *update);

private Q_SLOTS:
    void systemUpdatePaused(int value);

private:
    QHash<QString, Update*> m_apps;
    int m_downloadMode;
    QVariantList m_model;
    SystemUpdate m_systemUpdate;

    void checkForUpdates();
};

}

#endif // UPDATEMANAGER_H
