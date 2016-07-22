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

using namespace UpdatePlugin;

MockSystemUpdate::MockSystemUpdate(MockUpdateModel *model, QObject *parent)
    : UpdatePlugin::SystemUpdate(model,
                                 new UpdateModelFilter(model, model),
                                 new UpdateModelFilter(model, model),
                                 new UpdateModelFilter(model, model),
                                 new UpdateModelFilter(model, model),
                                 nullptr, parent)
{
}

MockSystemUpdate *MockSystemUpdate::m_instance = 0;

MockSystemUpdate *MockSystemUpdate::instance()
{
    MockUpdateModel *model = new MockUpdateModel();
    if (!m_instance) m_instance = new MockSystemUpdate(model);
    model->setParent(m_instance);
    return m_instance;
}

void MockSystemUpdate::mockIsCheckRequired(const bool isRequired)
{
    m_checkRequired = isRequired;
}

bool MockSystemUpdate::isCheckRequired()
{
    return m_checkRequired;
}
