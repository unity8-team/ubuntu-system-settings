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


} // DisplayPlugin
