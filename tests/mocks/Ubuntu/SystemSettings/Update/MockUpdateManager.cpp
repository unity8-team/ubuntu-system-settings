/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MockUpdateManager.h"

MockUpdateManager::MockUpdateManager(QObject *parent)
    : QObject(parent)
    , m_systemImage(new MockSystemImage(parent))
{
}

MockUpdateManager *MockUpdateManager::m_instance = 0;

MockUpdateManager *MockUpdateManager::instance()
{
    if (!m_instance) m_instance = new MockUpdateManager;

    return m_instance;
}

MockUpdateManager::~MockUpdateManager()
{
}


bool MockUpdateManager::online() const
{
    return false;
}

void MockUpdateManager::setOnline(const bool online)
{
}

bool MockUpdateManager::authenticated() const
{
    return false;
}

bool MockUpdateManager::haveSufficientPower() const
{
    return false;
}

bool MockUpdateManager::haveSystemUpdate() const
{
    return false;
}

MockSystemImage* MockUpdateManager::systemImageBackend() const
{
    return m_systemImage;
}

void MockUpdateManager::setHaveSufficientPower(const bool haveSufficientPower)
{
}

int MockUpdateManager::updatesCount() const
{
    return 0;
}

int MockUpdateManager::downloadMode()
{
    return m_systemImage->downloadMode();
}

void MockUpdateManager::setDownloadMode(const int &downloadMode)
{
}

MockUpdateManager::ManagerStatus MockUpdateManager::managerStatus() const
{
    return ManagerStatus::Idle;
}

MockClickUpdateModel *MockUpdateManager::installedClickUpdates()
{
    return new MockClickUpdateModel(this);
}

MockClickUpdateModel *MockUpdateManager::activeClickUpdates()
{
    return new MockClickUpdateModel(this);
}

void MockUpdateManager::checkForUpdates()
{
}

void MockUpdateManager::cancelCheckForUpdates()
{

}

void MockUpdateManager::retryClickPackage(const QString &packageName, const int &revision)
{
}
