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

#include "mir_output.h"

#include "../helpers.h"
#include "../outputmode/mir_outputmode.h"

#include <QDebug>

namespace DisplayPlugin
{
MirOutputImpl::MirOutputImpl(MirOutput *output)
    : m_output(output)
{
}

MirOutputImpl::~MirOutputImpl()
{
}

QSharedPointer<OutputMode> MirOutputImpl::getPreferredMode()
{
    auto mode = mir_output_get_preferred_mode(m_output);
    return QSharedPointer<OutputMode>(new MirOutputModeImpl(mode));
}

QSharedPointer<OutputMode> MirOutputImpl::getCurrentMode()
{
    auto mode = mir_output_get_current_mode(m_output);
    return QSharedPointer<OutputMode>(new MirOutputModeImpl(mode));
}

QList<QSharedPointer<OutputMode>> MirOutputImpl::getAvailableModes()
{
    QList<QSharedPointer<OutputMode>> list;

    for (int i = 0; i < mir_output_get_num_modes(m_output); i++) {
        auto mirMode = mir_output_get_mode(m_output, i);
        auto mode = QSharedPointer<OutputMode>(new MirOutputModeImpl(mirMode));
        list.append(mode);
    }
    return list;
}

int MirOutputImpl::getNumPixelFormats()
{

}

int MirOutputImpl::getId()
{
    return mir_output_get_id(m_output);
}

Enums::OutputType MirOutputImpl::getType()
{
    return Helpers::mirOutputTypeToOutputType(
        mir_output_get_type(m_output)
    );
}

int MirOutputImpl::getPositionX()
{

}

int MirOutputImpl::getPositionY()
{

}

Enums::ConnectionState MirOutputImpl::getConnectionState()
{
    return Helpers::mirConnectionStateToConnectionState(
        mir_output_get_connection_state(m_output)
    );
}

bool MirOutputImpl::isEnabled()
{
    return mir_output_is_enabled(m_output);
}

QString MirOutputImpl::getModel()
{
    return QString::fromUtf8(mir_output_get_model(m_output));
}

int MirOutputImpl::getPhysicalWidthMm()
{

}

int MirOutputImpl::getPhysicalHeightMm()
{

}

Enums::PowerMode MirOutputImpl::getPowerMode()
{
    return Helpers::mirPowerModeToPowerMode(
        mir_output_get_power_mode(m_output)
    );
}

Enums::Orientation MirOutputImpl::getOrientation()
{
    return Helpers::mirOrientationToOrientation(
        mir_output_get_orientation(m_output)
    );
}

float MirOutputImpl::getScaleFactor()
{
    return mir_output_get_scale_factor(m_output);
}

uint8_t const* MirOutputImpl::getEdid()
{

}

bool MirOutputImpl::isGammaSupported()
{

}

uint32_t MirOutputImpl::getGammaSize()
{

}

void MirOutputImpl::getGamma(uint16_t* red,
                             uint16_t* green,
                             uint16_t* blue,
                             uint32_t  size)
{
}

void MirOutputImpl::setCurrentMode(const QSharedPointer<OutputMode> &mode)
{
    auto mirMode = (MirOutputModeImpl*) mode.data();
    mir_output_set_current_mode(m_output, mirMode->getMirOutputMode());
}

void MirOutputImpl::setPosition(int x, int y)
{

}

void MirOutputImpl::enable()
{
    mir_output_enable(m_output);
}

void MirOutputImpl::disable()
{
    mir_output_disable(m_output);
}

void MirOutputImpl::setGamma(uint16_t const* red,
                             uint16_t const* green,
                             uint16_t const* blue,
                             uint32_t size)
{
}
                          ;
void MirOutputImpl::setPowerMode(Enums::PowerMode mode)
{
    auto mirMode = Helpers::powerModeToMirPowerMode(mode);
    mir_output_set_power_mode(m_output, mirMode);
}

void MirOutputImpl::setOrientation(Enums::Orientation orientation)
{
    auto mirOrientation = Helpers::orientationToMirOrientation(orientation);
    mir_output_set_orientation(m_output, mirOrientation);
}

void MirOutputImpl::setScaleFactor(float scale)
{
    qWarning() << Q_FUNC_INFO << "setting" << scale;
    mir_output_set_scale_factor(m_output, scale);
}

} // DisplayPlugin
