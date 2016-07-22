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
    : QObject(parent)
    , m_model(new UpdateModel(this))
    , m_pending(new UpdateModelFilter(m_model, this))
    , m_clicks(new UpdateModelFilter(m_model, this))
    , m_images(new UpdateModelFilter(m_model, this))
    , m_installed(new UpdateModelFilter(m_model, this))
    , m_nam(new Network::ManagerImpl(this))
{
    init();
}

SystemUpdate::SystemUpdate(UpdateModel *model,
                           UpdateModelFilter *pending,
                           UpdateModelFilter *clicks,
                           UpdateModelFilter *images,
                           UpdateModelFilter *installed,
                           Network::Manager *nam,
                           QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_pending(pending)
    , m_clicks(clicks)
    , m_images(images)
    , m_installed(installed)
    , m_nam(nam)
{
    init();
}

void SystemUpdate::init()
{
    m_pending->filterOnInstalled(false);

    m_clicks->filterOnKind((uint) Update::Kind::KindClick);
    m_clicks->filterOnInstalled(false);

    m_images->filterOnKind((uint) Update::Kind::KindImage);
    m_images->filterOnInstalled(false);

    m_installed->filterOnInstalled(true);
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

bool SystemUpdate::isCheckRequired()
{
    // Spec says that a manual check should not happen if a check was
    // completed less than 30 minutes ago.
    QDateTime now = QDateTime::currentDateTimeUtc().addSecs(-1800); // 30 mins
    // TODO: do not break Demeter's law
    return m_model->db()->lastCheckDate() < now;
}

void SystemUpdate::checkCompleted()
{
    m_model->db()->setLastCheckDate(QDateTime::currentDateTime());
}
} // UpdatePlugin
