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

#include "mirfocuscontroller.h"

#include "mirsurfaceinterface.h"

// mirserver
#include <logging.h>

namespace unityapp = unity::shell::application;
using namespace qtmir;

#define DEBUG_MSG qCDebug(QTMIR_SURFACES).nospace() << "MirFocusController::" << __func__

MirFocusController *MirFocusController::m_instance = nullptr;

void MirFocusController::setFocusedSurface(unityapp::MirSurfaceInterface *unityAppSurface)
{
    auto surface = static_cast<qtmir::MirSurfaceInterface*>(unityAppSurface);

    if (m_focusedSurface == surface) {
        return;
    }

    DEBUG_MSG << "(" << surface << ")";

    m_previouslyFocusedSurface = m_focusedSurface;
    m_focusedSurface = surface;

    if (m_previouslyFocusedSurface) {
        m_previouslyFocusedSurface->setFocused(false);
    }

    if (m_focusedSurface) {
        m_focusedSurface->setFocused(true);
        m_focusedSurface->raise();
    }

    if (m_previouslyFocusedSurface != m_focusedSurface) {
        Q_EMIT focusedSurfaceChanged();
    }
}

unity::shell::application::MirSurfaceInterface* MirFocusController::focusedSurface() const
{
    return m_focusedSurface;
}

MirFocusController* MirFocusController::instance()
{
    if (!m_instance) {
        m_instance = new MirFocusController;
    }
    return m_instance;
}
