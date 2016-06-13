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

#include "MockUpdateManager.h"

MockUpdateManager::MockUpdateManager(QObject *parent)
    : QObject(parent)
    , m_authenticated(false)
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

bool MockUpdateManager::authenticated(){
    return m_authenticated;
}

void MockUpdateManager::checkForClickUpdates()
{
}

void MockUpdateManager::cancelCheckForClickUpdates()
{

}

void MockUpdateManager::retryClickPackage(const QString &packageName, const int &revision)
{
}

void MockUpdateManager::clickUpdateInstalled(const QString &packageName, const int &revision)
{

}

void MockUpdateManager::mockAuthenticate(const bool authenticated)
{
    m_authenticated = authenticated;
    Q_EMIT (authenticatedChanged());
}

void MockUpdateManager::mockServerError()
{
    Q_EMIT (serverError());
}

void MockUpdateManager::mockNetworkError()
{
    Q_EMIT (networkError());
}

void MockUpdateManager::mockClickUpdateCheckComplete()
{
    Q_EMIT (clickUpdateCheckCompleted());
}

void MockUpdateManager::mockClickUpdateCheckStarted()
{
    Q_EMIT (clickUpdateCheckStarted());
}
