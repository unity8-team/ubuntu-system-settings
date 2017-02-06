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

#include "helpers.h"

namespace DisplayPlugin
{
Enums::Orientation Helpers::mirOrientationToOrientation(const MirOrientation &mirOrientation)
{
    Enums::Orientation orientation;
    switch (mirOrientation) {
    case mir_orientation_normal:
        orientation = Enums::Orientation::NormalOrientation;
        break;
    case mir_orientation_left:
        orientation = Enums::Orientation::PortraitModeOrientation;
        break;
    case mir_orientation_inverted:
        orientation = Enums::Orientation::LandscapeInvertedModeOrientation;
        break;
    case mir_orientation_right:
        orientation = Enums::Orientation::PortraitInvertedModeOrientation;
        break;
    default:
        orientation = Enums::Orientation::NormalOrientation;
    }
    return orientation;
}

MirOrientation Helpers::orientationToMirOrientation(const Enums::Orientation &orientation)
{
    MirOrientation mirOrientation;
    switch (orientation) {
    case Enums::Orientation::NormalOrientation:
        mirOrientation = mir_orientation_normal;
        break;
    case Enums::Orientation::PortraitModeOrientation:
        mirOrientation = mir_orientation_left;
        break;
    case Enums::Orientation::LandscapeInvertedModeOrientation:
        mirOrientation = mir_orientation_inverted;
        break;
    case Enums::Orientation::PortraitInvertedModeOrientation:
        mirOrientation = mir_orientation_right;
        break;
    default:
        mirOrientation = mir_orientation_normal;
    }
    return mirOrientation;
}

QString Helpers::typeToString(const Enums::OutputType &type)
{
    QString ret;
    switch (type) {
    default:
    case Enums::OutputType::OutputTypeUnknown:
        ret = "";
        break;
    case Enums::OutputType::OutputTypeVga:
        ret = "vga";
        break;
    case Enums::OutputType::OutputTypeDvii:
        ret = "dvii";
        break;
    case Enums::OutputType::OutputTypeDvid:
        ret = "dvid";
        break;
    case Enums::OutputType::OutputTypeDvia:
        ret = "dvia";
        break;
    case Enums::OutputType::OutputTypeComposite:
        ret = "composite";
        break;
    case Enums::OutputType::OutputTypeSvideo:
        ret = "svideo";
        break;
    case Enums::OutputType::OutputTypeLvds:
        ret = "lvds";
        break;
    case Enums::OutputType::OutputTypeComponent:
        ret = "component";
        break;
    case Enums::OutputType::OutputTypeNinepindin:
        ret = "ninepindin";
        break;
    case Enums::OutputType::OutputTypeDisplayport:
        ret = "displayport";
        break;
    case Enums::OutputType::OutputTypeHdmia:
        ret = "hdmia";
        break;
    case Enums::OutputType::OutputTypeHdmib:
        ret = "hdmib";
        break;
    case Enums::OutputType::OutputTypeTv:
        ret = "tv";
        break;
    case Enums::OutputType::OutputTypeEdp:
        ret = "edp";
        break;
    case Enums::OutputType::OutputTypeVirtual:
        ret = "virtual";
        break;
    case Enums::OutputType::OutputTypeDsi:
        ret = "dsi";
        break;
    case Enums::OutputType::OutputTypeDpi:
        ret = "dpi";
        break;
    }

    return ret;
}

Enums::PowerMode Helpers::mirPowerModeToPowerMode(const MirPowerMode &mode)
{
    switch(mode) {
    case mir_power_mode_on:
        return Enums::PowerMode::OnMode;
    case mir_power_mode_standby:
        return Enums::PowerMode::StandbyMode;
    case mir_power_mode_suspend:
        return Enums::PowerMode::SuspendMode;
    case mir_power_mode_off:
        return Enums::PowerMode::OffMode;
    }
    return Enums::PowerMode::OffMode;
}

MirPowerMode Helpers::powerModeToMirPowerMode(const Enums::PowerMode &mode)
{
    switch(mode) {
    case Enums::PowerMode::OnMode:
        return mir_power_mode_on;
    case Enums::PowerMode::StandbyMode:
        return mir_power_mode_standby;
    case Enums::PowerMode::SuspendMode:
        return mir_power_mode_suspend;
    case Enums::PowerMode::OffMode:
        return mir_power_mode_off;
    }
    return mir_power_mode_off;
}

Enums::OutputType Helpers::mirOutputTypeToOutputType(const MirOutputType &type)
{
    switch (type) {
    default:
    case MirOutputType::mir_output_type_unknown:
        return Enums::OutputType::OutputTypeUnknown;
    case MirOutputType::mir_output_type_vga:
        return Enums::OutputType::OutputTypeVga;
    case MirOutputType::mir_output_type_dvii:
        return Enums::OutputType::OutputTypeDvii;
    case MirOutputType::mir_output_type_dvid:
        return Enums::OutputType::OutputTypeDvid;
    case MirOutputType::mir_output_type_dvia:
        return Enums::OutputType::OutputTypeDvia;
    case MirOutputType::mir_output_type_composite:
        return Enums::OutputType::OutputTypeComposite;
    case MirOutputType::mir_output_type_svideo:
        return Enums::OutputType::OutputTypeSvideo;
    case MirOutputType::mir_output_type_lvds:
        return Enums::OutputType::OutputTypeLvds;
    case MirOutputType::mir_output_type_component:
        return Enums::OutputType::OutputTypeComponent;
    case MirOutputType::mir_output_type_ninepindin:
        return Enums::OutputType::OutputTypeNinepindin;
    case MirOutputType::mir_output_type_displayport:
        return Enums::OutputType::OutputTypeDisplayport;
    case MirOutputType::mir_output_type_hdmia:
        return Enums::OutputType::OutputTypeHdmia;
    case MirOutputType::mir_output_type_hdmib:
        return Enums::OutputType::OutputTypeHdmib;
    case MirOutputType::mir_output_type_tv:
        return Enums::OutputType::OutputTypeTv;
    case MirOutputType::mir_output_type_edp:
        return Enums::OutputType::OutputTypeEdp;
    case MirOutputType::mir_output_type_virtual:
        return Enums::OutputType::OutputTypeVirtual;
    case MirOutputType::mir_output_type_dsi:
        return Enums::OutputType::OutputTypeDsi;
    case MirOutputType::mir_output_type_dpi:
        return Enums::OutputType::OutputTypeDpi;
    }
}

Enums::ConnectionState Helpers::mirConnectionStateToConnectionState(const MirOutputConnectionState &state)
{
    switch (state) {
    default:
    case mir_output_connection_state_disconnected:
        return Enums::ConnectionState::Disconnected;
    case mir_output_connection_state_connected:
        return Enums::ConnectionState::Connected;
    case mir_output_connection_state_unknown:
        return Enums::ConnectionState::Unknown;
    }
}
} // DisplayPlugin
