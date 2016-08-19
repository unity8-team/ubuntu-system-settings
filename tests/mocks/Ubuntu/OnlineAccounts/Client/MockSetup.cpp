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

#include "MockSetup.h"

namespace OnlineAccountsClient {

void MockSetup::setApplicationId(const QString &applicationId)
{
    m_applicationId = applicationId;
    Q_EMIT applicationIdChanged();
}

QString MockSetup::applicationId() const
{
    return m_applicationId;
}

void MockSetup::setProviderId(const QString &providerId)
{
    m_providerId = providerId;
    Q_EMIT providerIdChanged();
}

QString MockSetup::providerId() const
{
    return m_providerId;
}

void MockSetup::exec()
{
    m_execCalled = true;
}

bool MockSetup::execCalled()
{
    return m_execCalled;
}

void MockSetup::mockFinished(const QVariantMap &reply)
{
    Q_EMIT finished(reply);
}
}
