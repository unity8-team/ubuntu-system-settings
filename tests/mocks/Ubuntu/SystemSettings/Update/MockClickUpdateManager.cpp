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
#include "MockClickUpdateManager.h"

MockClickUpdateManager::MockClickUpdateManager(QObject *parent)
    : QObject(parent)
    , m_authenticated(false)
    , m_checking(false)
{
}

void MockClickUpdateManager::check()
{
}

void MockClickUpdateManager::check(const QString &packageName)
{
}

void MockClickUpdateManager::cancel()
{
}

void MockClickUpdateManager::clickUpdateInstalled(const QString &packageName, const int &revision)
{
}

bool MockClickUpdateManager::authenticated()
{
    return m_authenticated;
}

void MockClickUpdateManager::mockCheckStarted()
{
    Q_EMIT (checkStarted());
}

void MockClickUpdateManager::mockCheckComplete()
{
    Q_EMIT (checkCompleted());
}

void MockClickUpdateManager::mockCheckCanceled()
{
    Q_EMIT (checkCanceled());
}

void MockClickUpdateManager::mockCheckFailed()
{
    Q_EMIT (checkFailed());
}

void MockClickUpdateManager::mockAuthenticated(const bool authenticated)
{
    m_authenticated = authenticated;
    Q_EMIT(authenticatedChanged());
}

void MockClickUpdateManager::mockNetworkError()
{
    Q_EMIT (networkError());
}

void MockClickUpdateManager::mockServerError()
{
    Q_EMIT (serverError());
}

void MockClickUpdateManager::mockCredentialError()
{
    Q_EMIT (credentialError());
}

