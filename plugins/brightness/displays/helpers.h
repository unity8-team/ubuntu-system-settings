#ifndef DISPLAY_PLUGIN_HELPERS_H
#define DISPLAY_PLUGIN_HELPERS_H

#include "display.h"

#include <mir_toolkit/mir_client_library.h>

namespace DisplayPlugin
{
struct Helpers
{
public:
    static Display::Orientation mirOrientationToOritentation(const MirOrientation &mirOrientation);
    static MirOrientation orientationToMirOrientation(const Display::Orientation &orientation);
    static QString mirTypeToString(const MirDisplayOutputType &type);
    static QString mirModeToString(const MirDisplayOutputType &type);
    static Display::PowerMode mirPowerModeToPowerMode(const MirPowerMode &mode);
    static MirPowerMode powerModeToMirPowerMode(const Display::PowerMode &mode);

    // Return a sorted list of modes in horizontalxverticalxrate format.
    // static QString mirModesToStringList(const MirDisplayOutputType &type);
};
} // DisplayPlugin

#endif // DISPLAY_PLUGIN_HELPERS_H
