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

#ifndef QTMIR_MIRFOCUSCONTROLLER_H
#define QTMIR_MIRFOCUSCONTROLLER_H

// unity-api
#include <unity/shell/application/MirFocusControllerInterface.h>

#include <QPointer>

#include "mirsurfaceinterface.h"

namespace qtmir {

class MirSurfaceInterface;

class MirFocusController : public unity::shell::application::MirFocusControllerInterface
{
    Q_OBJECT
public:
    MirFocusController(){}
    static MirFocusController* instance();

    void setFocusedSurface(unity::shell::application::MirSurfaceInterface *surface) override;
    unity::shell::application::MirSurfaceInterface* focusedSurface() const override;
    MirSurfaceInterface* previouslyFocusedSurface() { return m_previouslyFocusedSurface.data(); }
private:
    static MirFocusController *m_instance;
    QPointer<MirSurfaceInterface> m_previouslyFocusedSurface;
    QPointer<MirSurfaceInterface> m_focusedSurface;
};

} // namespace qtmir

#endif // QTMIR_MIRFOCUSCONTROLLER_H
