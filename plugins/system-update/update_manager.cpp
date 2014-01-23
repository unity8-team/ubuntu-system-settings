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
#include <QDebug>

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
                  SIGNAL(systemUpdateFailed()));
}

UpdateManager::~UpdateManager()
{
}

void UpdateManager::checkUpdates()
{
    m_model.clear();
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

void UpdateManager::startDownload(const QString &packagename)
{
    qDebug() << "Download Package:" << packagename;
    m_apps[packagename]->setUpdateState(true);
    m_systemUpdate.downloadUpdate();
}

void UpdateManager::pauseDownload(const QString &packagename)
{
    qDebug() << "Pause Package:" << packagename;
    m_apps[packagename]->setUpdateState(false);
    m_systemUpdate.pauseDownload();
}

}
