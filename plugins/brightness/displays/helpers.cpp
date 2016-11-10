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

#include "helpers.h"

namespace DisplayPlugin
{
Display::Orientation Helpers::mirOrientationToOritentation(const MirOrientation &mirOrientation)
{
    Display::Orientation orientation;
    switch (mirOrientation) {
    case mir_orientation_normal:
        orientation = Display::Orientation::NormalOrientation;
        break;
    case mir_orientation_left:
        orientation = Display::Orientation::PortraitModeOrientation;
        break;
    case mir_orientation_inverted:
        orientation = Display::Orientation::LandscapeInvertedModeOrientation;
        break;
    case mir_orientation_right:
        orientation = Display::Orientation::PortraitInvertedModeOrientation;
        break;
    default:
        orientation = Display::Orientation::NormalOrientation;
    }
    return orientation;
}

MirOrientation Helpers::orientationToMirOrientation(const Display::Orientation &orientation)
{
    MirOrientation mirOrientation;
    switch (orientation) {
    case Display::Orientation::NormalOrientation:
        mirOrientation = mir_orientation_normal;
        break;
    case Display::Orientation::PortraitModeOrientation:
        mirOrientation = mir_orientation_left;
        break;
    case Display::Orientation::LandscapeInvertedModeOrientation:
        mirOrientation = mir_orientation_inverted;
        break;
    case Display::Orientation::PortraitInvertedModeOrientation:
        mirOrientation = mir_orientation_right;
        break;
    default:
        mirOrientation = mir_orientation_normal;
    }
    return mirOrientation;
}

QString Helpers::mirTypeToString(const MirDisplayOutputType &type)
{
    QString ret;
    switch (type) {
    default:
    case mir_display_output_type_unknown:
        ret = "";
        break;
    case mir_display_output_type_vga:
        ret = "vga";
        break;
    case mir_display_output_type_dvii:
        ret = "dvii";
        break;
    case mir_display_output_type_dvid:
        ret = "dvid";
        break;
    case mir_display_output_type_dvia:
        ret = "dvia";
        break;
    case mir_display_output_type_composite:
        ret = "composite";
        break;
    case mir_display_output_type_svideo:
        ret = "svideo";
        break;
    case mir_display_output_type_lvds:
        ret = "lvds";
        break;
    case mir_display_output_type_component:
        ret = "component";
        break;
    case mir_display_output_type_ninepindin:
        ret = "ninepindin";
        break;
    case mir_display_output_type_displayport:
        ret = "displayport";
        break;
    case mir_display_output_type_hdmia:
        ret = "hdmia";
        break;
    case mir_display_output_type_hdmib:
        ret = "hdmib";
        break;
    case mir_display_output_type_tv:
        ret = "tv";
        break;
    case mir_display_output_type_edp:
        ret = "edp";
        break;
    }

    return ret;
}

Display::PowerMode Helpers::mirPowerModeToPowerMode(const MirPowerMode &mode)
{
    switch(mode) {
    case mir_power_mode_on:
        return Display::PowerMode::OnMode;
    case mir_power_mode_standby:
        return Display::PowerMode::StandbyMode;
    case mir_power_mode_suspend:
        return Display::PowerMode::SuspendMode;
    case mir_power_mode_off:
        return Display::PowerMode::OffMode;
    }
    return Display::PowerMode::OffMode;
}

MirPowerMode Helpers::powerModeToMirPowerMode(const Display::PowerMode &mode)
{
    switch(mode) {
    case Display::PowerMode::OnMode:
        return mir_power_mode_on;
    case Display::PowerMode::StandbyMode:
        return mir_power_mode_standby;
    case Display::PowerMode::SuspendMode:
        return mir_power_mode_suspend;
    case Display::PowerMode::OffMode:
        return mir_power_mode_off;
    }
    return mir_power_mode_off;
}
} // DisplayPlugin
