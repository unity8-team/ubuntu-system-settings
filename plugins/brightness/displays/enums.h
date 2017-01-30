/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
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

#ifndef DISPLAYS_ENUMS_H
#define DISPLAYS_ENUMS_H

#include <QObject>

namespace DisplayPlugin
{
class Q_DECL_EXPORT Enums
{
    Q_GADGET
public:
    enum ConnectionState
    {
        Disconnected = 0,
        Connected,
        Unknown
    };
    Q_ENUM(ConnectionState);

    enum class Orientation : uint {
        NormalOrientation, PortraitModeOrientation,
        LandscapeInvertedModeOrientation,
        PortraitInvertedModeOrientation
    };

    Q_ENUM(Orientation)
    enum class PowerMode : uint {
        OnMode, StandbyMode, SuspendMode, OffMode
    };
    Q_ENUM(PowerMode)
};
} // DisplayPlugin

#endif // DISPLAYS_ENUMS_H
