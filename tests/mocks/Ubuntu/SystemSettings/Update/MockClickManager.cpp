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
#include "MockClickManager.h"
#include <QDebug>

MockClickManager::MockClickManager(QObject *parent)
    : QObject(parent)
    , m_authenticated(true)
    , m_checking(false)
{
}

void MockClickManager::check()
{
    m_checking = true;
}

void MockClickManager::check(const QString &packageName)
{
}

void MockClickManager::cancel()
{
    m_checking = false;
}

void MockClickManager::clickUpdateInstalled(const QString &packageName, const int &revision)
{
}

bool MockClickManager::authenticated()
{
    return m_authenticated;
}

void MockClickManager::mockCheckStarted()
{
    check();
    Q_EMIT (checkStarted());
}

void MockClickManager::mockCheckComplete()
{
    cancel();
    Q_EMIT (checkCompleted());
}

void MockClickManager::mockCheckCanceled()
{
    cancel();
    Q_EMIT (checkCanceled());
}

void MockClickManager::mockCheckFailed()
{
    cancel();
    Q_EMIT (checkFailed());
}

void MockClickManager::mockAuthenticated(const bool authenticated)
{
    m_authenticated = authenticated;
    Q_EMIT(authenticatedChanged());
}

void MockClickManager::mockNetworkError()
{
    Q_EMIT (networkError());
}

void MockClickManager::mockServerError()
{
    Q_EMIT (serverError());
}

void MockClickManager::mockCredentialError()
{
    Q_EMIT (credentialError());
}

bool MockClickManager::isChecking() const
{
    return m_checking;
}
