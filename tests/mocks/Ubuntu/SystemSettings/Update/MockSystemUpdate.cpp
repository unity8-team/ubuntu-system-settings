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
#include "MockSystemUpdate.h"
#include "updatemodel.h"
#include "fakeclickmanager.h"
#include "fakeimagemanager.h"

using namespace UpdatePlugin;

MockSystemUpdate::MockSystemUpdate(QObject *parent)
    : UpdatePlugin::SystemUpdate(new MockUpdateModel(this),
                                 nullptr, // We don't need a NAM.
                                 new UpdateModelFilter(m_model, m_model),
                                 new UpdateModelFilter(m_model, m_model),
                                 new UpdateModelFilter(m_model, m_model),
                                 new UpdateModelFilter(m_model, m_model),
                                 new MockImageManager(this),
                                 new MockClickManager(this),
                                 parent)
{
}

void MockSystemUpdate::mockIsCheckRequired(const bool isRequired)
{
    m_checkRequired = isRequired;
}

bool MockSystemUpdate::isCheckRequired()
{
    return m_checkRequired;
}
