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

#include "systemupdate.h"
#include "click/manager_impl.h"
#include "image/imagemanager_impl.h"
#include "network/accessmanager_impl.h"

#include <QDateTime>
#include <QQmlEngine>

namespace UpdatePlugin
{
SystemUpdate::SystemUpdate(QObject *parent)
    : QObject(parent)
    , m_model(new UpdateModel(this))
    , m_nam(new Network::ManagerImpl(this))
    , m_imageManager(new Image::ManagerImpl(m_model, this))
    , m_clickManager(new Click::ManagerImpl(m_model, m_nam, this))
{
   init();
}

SystemUpdate::SystemUpdate(UpdateModel *model,
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

void SystemUpdate::init()
{
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

UpdateModel* SystemUpdate::updates()
{
    QQmlEngine::setObjectOwnership(m_model, QQmlEngine::CppOwnership);
    return m_model;
}

UpdateModelFilter* SystemUpdate::pendingUpdates()
{
    auto ret = &m_pending;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

UpdateModelFilter* SystemUpdate::clickUpdates()
{
    auto ret = &m_clicks;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

UpdateModelFilter* SystemUpdate::imageUpdates()
{
    auto ret = &m_images;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

UpdateModelFilter* SystemUpdate::installedUpdates()
{
    auto ret = &m_installed;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

SystemUpdate::Status SystemUpdate::status() const
{
    return m_status;
}

void SystemUpdate::calculateStatus()
{
    Status status;
    bool clickCheck = m_clickManager->checkingForUpdates();
    bool imageCheck = m_imageManager->checkingForUpdates();
    if (clickCheck && imageCheck) {
        status = Status::StatusCheckingAllUpdates;
    } else if (clickCheck) {
        status = Status::StatusCheckingClickUpdates;
    } else if (imageCheck) {
        status = Status::StatusCheckingSystemUpdates;
    } else {
        status = Status::StatusIdle;
    }
    setStatus(status);
}

void SystemUpdate::setStatus(const Status &status)
{
    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged();
    }
}

bool SystemUpdate::isCheckRequired()
{
    QDateTime now = QDateTime::currentDateTimeUtc().addSecs(-1800); // 30 mins
    return m_model->db()->lastCheckDate() < now;
}

bool SystemUpdate::authenticated()
{
    return m_clickManager->authenticated();
}

void SystemUpdate::handleCheckCompleted()
{
    m_model->db()->setLastCheckDate(QDateTime::currentDateTime());
}

void SystemUpdate::handleNetworkError()
{
    setStatus(Status::StatusNetworkError);
}

void SystemUpdate::handleServerError()
{
    setStatus(Status::StatusServerError);
}

void SystemUpdate::check(const Check check)
{
    switch (check) {
    case Check::CheckAutomatic:
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

void SystemUpdate::cancel()
{
    m_imageManager->cancel();
    m_clickManager->cancel();
}

bool SystemUpdate::launch(const QString &identifier)
{
    // We can currently launch click packages only.
    return m_clickManager->launch(identifier);
}

void SystemUpdate::retry(const QString &identifier, const uint &revision)
{
    /* QML talks to SystemImage directly when retrying, so we only have to deal
    with clicks here. */
    m_clickManager->retry(identifier, revision);
}

} // UpdatePlugin
