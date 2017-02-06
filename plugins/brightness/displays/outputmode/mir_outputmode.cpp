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

#include "mir_outputmode.h"
#include "../../../../src/i18n.h"

namespace DisplayPlugin
{
MirOutputModeImpl::MirOutputModeImpl(MirOutputMode const *outputMode)
    : m_outputMode(outputMode)
{
}

MirOutputModeImpl::~MirOutputModeImpl()
{
}

int MirOutputModeImpl::getWidth()
{
    return mir_output_mode_get_width(m_outputMode);
}

int MirOutputModeImpl::getHeight()
{
    return mir_output_mode_get_height(m_outputMode);
}

double MirOutputModeImpl::getRefreshRate()
{
    return mir_output_mode_get_refresh_rate(m_outputMode);
}

QString MirOutputModeImpl::toString()
{
    /* TRANSLATORS: %1 refer to the amount of horizontal pixels in a
    display resolution, and %2 to the vertical pixels. E.g. 1200x720.
    %3 is the refresh rate in hz. */
    return SystemSettings::_("%1×%2 @ %3hz")
        .arg(getWidth())
        .arg(getHeight())
        .arg(getRefreshRate());
}

MirOutputMode const* MirOutputModeImpl::getMirOutputMode()
{
    return m_outputMode;
}
} // DisplayPlugin
