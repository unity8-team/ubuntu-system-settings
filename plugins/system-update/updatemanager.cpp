/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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

#include "updatemanager.h"
#include "click/manager_impl.h"
#include "image/imagemanager_impl.h"
#include "network/accessmanager_impl.h"

#include <QDateTime>
#include <QQmlEngine>

namespace UpdatePlugin
{
UpdateManager::UpdateManager(QObject *parent)
    : QObject(parent)
    , m_model(new UpdateModel(this))
    , m_nam(new Network::ManagerImpl(this))
    , m_imageManager(new Image::ManagerImpl(m_model, this))
    , m_clickManager(new Click::ManagerImpl(m_model, m_nam, this))
{
   init();
}

UpdateManager::UpdateManager(UpdateModel *model,
                           Network::Manager *nam,
                           Image::Manager *imageManager,
                           Click::Manager *clickManager,
                           QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_nam(nam)
    , m_imageManager(imageManager)
    , m_clickManager(clickManager)
{
    init();
}

void UpdateManager::init()
{
    m_model->db()->pruneDb();
    m_pending.setSourceModel(m_model);
    m_pending.filterOnInstalled(false);
    m_clicks.setSourceModel(m_model);
    m_clicks.filterOnKind((uint) Update::Kind::KindClick);
    m_clicks.filterOnInstalled(false);
    m_images.setSourceModel(m_model);
    m_images.filterOnKind((uint) Update::Kind::KindImage);
    m_images.filterOnInstalled(false);
    m_installed.setSourceModel(m_model);
    m_installed.filterOnInstalled(true);

    connect(m_clickManager, SIGNAL(checkingForUpdatesChanged()),
            this, SLOT(calculateStatus()));
    connect(m_clickManager, SIGNAL(checkCompleted()),
            this, SLOT(handleCheckCompleted()));
    connect(m_clickManager, SIGNAL(networkError()),
            this, SLOT(handleNetworkError()));
    connect(m_clickManager, SIGNAL(serverError()),
            this, SLOT(handleServerError()));
    connect(m_clickManager, SIGNAL(authenticatedChanged()),
            this, SIGNAL(authenticatedChanged()));
    connect(m_imageManager, SIGNAL(checkingForUpdatesChanged()),
            this, SLOT(calculateStatus()));
    connect(m_imageManager, SIGNAL(checkCompleted()),
            this, SLOT(handleCheckCompleted()));
}

UpdateModel* UpdateManager::updates()
{
    QQmlEngine::setObjectOwnership(m_model, QQmlEngine::CppOwnership);
    return m_model;
}

UpdateModelFilter* UpdateManager::pendingUpdates()
{
    auto ret = &m_pending;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

UpdateModelFilter* UpdateManager::clickUpdates()
{
    auto ret = &m_clicks;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

UpdateModelFilter* UpdateManager::imageUpdates()
{
    auto ret = &m_images;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

UpdateModelFilter* UpdateManager::installedUpdates()
{
    auto ret = &m_installed;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

UpdateManager::Status UpdateManager::status() const
{
    return m_status;
}

void UpdateManager::calculateStatus()
{
    Status status;
    bool clickCheck = m_clickManager->checkingForUpdates();
    bool imageCheck = m_imageManager->checkingForUpdates();
    if (clickCheck && imageCheck) {
        status = Status::StatusCheckingAllUpdates;
    } else if (clickCheck) {
        status = Status::StatusCheckingClickUpdates;
    } else if (imageCheck) {
        status = Status::StatusCheckingImageUpdates;
    } else {
        status = Status::StatusIdle;
    }
    setStatus(status);
}

void UpdateManager::setStatus(const Status &status)
{
    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged();
    }
}

bool UpdateManager::isCheckRequired()
{
    QDateTime now = QDateTime::currentDateTimeUtc().addSecs(-1800); // 30 mins
    return m_model->db()->lastCheckDate() < now;
}

bool UpdateManager::authenticated()
{
    return m_clickManager->authenticated();
}

void UpdateManager::handleCheckCompleted()
{
    m_model->db()->setLastCheckDate(QDateTime::currentDateTime());
}

void UpdateManager::handleNetworkError()
{
    setStatus(Status::StatusNetworkError);
}

void UpdateManager::handleServerError()
{
    setStatus(Status::StatusServerError);
}

void UpdateManager::check(const Check check)
{
    switch (check) {
    case Check::CheckIfNecessary:
        if (isCheckRequired()) {
            m_imageManager->check();
            m_clickManager->check();
        }
        break;
    case Check::CheckAll:
        m_imageManager->check();
        m_clickManager->check();
        break;
    case Check::CheckClick:
            m_clickManager->check();
        break;
    case Check::CheckImage:
            m_imageManager->check();
        break;
    }
}

void UpdateManager::cancel()
{
    m_imageManager->cancel();
    m_clickManager->cancel();
}

bool UpdateManager::launch(const QString &identifier)
{
    // We can currently launch click packages only.
    return m_clickManager->launch(identifier);
}

void UpdateManager::retry(const QString &identifier, const uint &revision)
{
    /* QML talks to SystemImage directly when retrying, so we only have to deal
    with clicks here. */
    m_clickManager->retry(identifier, revision);
}

void UpdateManager::remove(const QString &identifier, const uint &revision)
{
    m_model->remove(identifier, revision);
}
} // UpdatePlugin
