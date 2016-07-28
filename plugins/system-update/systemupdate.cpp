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

#include "click/manager_impl.h"
#include "image/imagemanager_impl.h"
#include "network/accessmanager_impl.h"
#include "systemupdate.h"

#include <QDateTime>
#include <QQmlEngine>

namespace UpdatePlugin
{
SystemUpdate *SystemUpdate::m_instance = 0;

SystemUpdate *SystemUpdate::instance()
{
    if (!m_instance) m_instance = new SystemUpdate;
    return m_instance;
}

void SystemUpdate::destroyInstance()
{
    delete m_instance;
    m_instance = nullptr;
}

SystemUpdate::SystemUpdate(QObject *parent)
    : SystemUpdate(new UpdateModel(this),
                   new UpdateModelFilter(m_model, this),
                   new UpdateModelFilter(m_model, this),
                   new UpdateModelFilter(m_model, this),
                   new UpdateModelFilter(m_model, this),
                   new Network::ManagerImpl(this),
                   new Image::ManagerImpl(m_model, this),
                   new Click::ManagerImpl(m_model, this), parent)
{
}

SystemUpdate::SystemUpdate(UpdateModel *model,
                           UpdateModelFilter *pending,
                           UpdateModelFilter *clicks,
                           UpdateModelFilter *images,
                           UpdateModelFilter *installed,
                           Network::Manager *nam,
                           Image::Manager *imageManager,
                           Click::Manager *clickManager,
                           QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_pending(pending)
    , m_clicks(clicks)
    , m_images(images)
    , m_installed(installed)
    , m_imageManager(imageManager)
    , m_clickManager(clickManager)
    , m_nam(nam)
{
    m_pending->filterOnInstalled(false);

    m_clicks->filterOnKind((uint) Update::Kind::KindClick);
    m_clicks->filterOnInstalled(false);

    m_images->filterOnKind((uint) Update::Kind::KindImage);
    m_images->filterOnInstalled(false);

    m_installed->filterOnInstalled(true);

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

UpdateModelFilter* SystemUpdate::pendingUpdates() const
{
    QQmlEngine::setObjectOwnership(m_pending, QQmlEngine::CppOwnership);
    return m_pending;
}

UpdateModelFilter* SystemUpdate::clickUpdates() const
{
    QQmlEngine::setObjectOwnership(m_clicks, QQmlEngine::CppOwnership);
    return m_clicks;
}

UpdateModelFilter* SystemUpdate::imageUpdates() const
{
    QQmlEngine::setObjectOwnership(m_images, QQmlEngine::CppOwnership);
    return m_images;
}

UpdateModelFilter* SystemUpdate::installedUpdates() const
{
    QQmlEngine::setObjectOwnership(m_installed, QQmlEngine::CppOwnership);
    return m_installed;
}

Network::Manager* SystemUpdate::nam()
{
    return m_nam;
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
    /* We treat these communication errors as fatal. The user needs to leave
    the System Update panel and come back in—recreating us. */
    if (m_status == Status::StatusNetworkError ||
        m_status == Status::StatusServerError) {
        return;
    }

    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged();
    }
}

bool SystemUpdate::isCheckRequired()
{
    /* Spec says that a manual check should not happen if a check was completed
    less than 30 minutes ago. */
    QDateTime now = QDateTime::currentDateTimeUtc().addSecs(-1800); // 30 mins
    // TODO: do not break Demeter's law
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

void SystemUpdate::retry(const QString &identifier, const uint &revision)
{
    m_clickManager->retry(identifier, revision);
}

void SystemUpdate::launch(const QString &identifier, const uint &revision)
{
    m_clickManager->launch(identifier, revision);
}

} // UpdatePlugin
