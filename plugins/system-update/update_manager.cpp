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

#include "update_manager.h"
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace UpdatePlugin {

UpdateManager::UpdateManager(QObject *parent):
    QObject(parent)
{
    // SYSTEM UPDATE
    QObject::connect(&m_systemUpdate, SIGNAL(updateAvailable(const QString&, Update*)),
                  this, SLOT(registerSystemUpdate(const QString&, Update*)));
    QObject::connect(&m_systemUpdate, SIGNAL(downloadModeChanged()),
                  SIGNAL(downloadModeChanged()));
    QObject::connect(&m_systemUpdate, SIGNAL(updateDownloaded()),
                  SIGNAL(systemUpdateDownloaded()));
    QObject::connect(&m_systemUpdate, SIGNAL(updateProcessFailed(const QString&)),
                  SIGNAL(updateProcessFailed(QString)));
    QObject::connect(&m_systemUpdate, SIGNAL(updateFailed(int, QString)),
                  SIGNAL(systemUpdateFailed(int, QString)));
    QObject::connect(&m_systemUpdate, SIGNAL(updatePaused(int)),
                  SLOT(systemUpdatePaused(int)));
}

UpdateManager::~UpdateManager()
{
}

void UpdateManager::checkUpdates()
{
    m_model.clear();
    m_apps.clear();
    Q_EMIT modelChanged();
    m_systemUpdate.checkForUpdate();
}

void UpdateManager::registerSystemUpdate(const QString& packageName, Update *update)
{
    if (update->updateRequired()) {
        m_apps[packageName] = update;
        m_model.insert(0, QVariant::fromValue(update));
        Q_EMIT modelChanged();
        Q_EMIT updateAvailableFound();
    } else {
        Q_EMIT updatesNotFound();
    }
}

void UpdateManager::systemUpdatePaused(int value)
{
    QString packagename("UbuntuImage");
    if (m_apps.contains(packagename)) {
        Update *update = m_apps[packagename];
        update->setSelected(true);
        update->setDownloadProgress(value);
    }
}

void UpdateManager::startDownload(const QString &packagename)
{
    m_apps[packagename]->setUpdateState(true);
    m_systemUpdate.downloadUpdate();
}

void UpdateManager::retryDownload(const QString &packagename)
{
    Update *update = m_apps.take(packagename);
    m_systemUpdate.cancelUpdate();
    m_model.removeAt(0);
    update->deleteLater();
    Q_EMIT modelChanged();
    m_systemUpdate.checkForUpdate();
}

void UpdateManager::pauseDownload(const QString &packagename)
{
    m_apps[packagename]->setUpdateState(false);
    m_systemUpdate.pauseDownload();
}

}
