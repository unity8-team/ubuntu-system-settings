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
#include "updatemodel.h"
#include "fakeclickmanager.h"
#include "fakeimagemanager.h"

using namespace UpdatePlugin;

MockUpdateManager::MockUpdateManager(QObject *parent)
    : UpdateManager(new MockUpdateModel(parent),
                    nullptr, // We don't need a NAM.
                    new MockImageManager(parent),
                    new MockClickManager(parent),
                    parent)
{
}

void MockUpdateManager::mockIsCheckRequired(const bool isRequired)
{
    m_checkRequired = isRequired;
}

bool MockUpdateManager::isCheckRequired()
{
    return m_checkRequired;
}

void MockUpdateManager::mockStatus(const uint &status)
{
    m_status = (UpdateManager::Status) status;
    Q_EMIT statusChanged();
}
