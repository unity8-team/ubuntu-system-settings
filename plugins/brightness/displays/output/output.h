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

#ifndef DISPLAYS_OUTPUT_H
#define DISPLAYS_OUTPUT_H

#include "../enums.h"
#include "../outputmode/outputmode.h"

#include <QObject>

namespace DisplayPlugin
{
/* Provides an abstraction of a video output, which can be a physical or
virtual video output (hdmi, screencast, etc.). */
class Q_DECL_EXPORT Output
{
    Q_GADGET
    // Q_DISABLE_COPY(Output)
public:
    virtual ~Output() {};
    // enum ConnectionState
    // {
    //     Disconnected = 0,
    //     Connected,
    //     Unknown
    // };
    // Q_ENUM(ConnectionState);

    virtual int getNumModes() = 0;
    // virtual OutputMode const* getMode(size_t index) = 0;
    // virtual OutputMode const* getPreferredMode() = 0;
    virtual size_t getPreferredModeIndex() = 0;
    // virtual OutputMode const* getCurrentMode() = 0;
    virtual size_t getCurrentModeIndex() = 0;
    virtual int getNumPixelFormats() = 0;
    // virtual MirPixelFormat getPixelFormat(size_t index) = 0;
    // virtual MirPixelFormat getCurrentPixelFormat() = 0;
    virtual int getId() = 0;
    // virtual  MirOutputType getType() = 0;
    virtual int getPositionX() = 0;
    virtual int getPositionY() = 0;
    virtual Enums::ConnectionState getConnectionState() = 0;
    virtual bool is_enabled() = 0;
    virtual char const* getModel() = 0;
    virtual int getPhysicalWidthMm() = 0;
    virtual int getPhysicalHeightMm() = 0;
    virtual Enums::PowerMode getPowerMode() = 0;
    virtual Enums::Orientation getOrientation() = 0;
    virtual float getScaleFactor() = 0;
    // virtual MirSubpixelArrangement getSubpixelArrangement() = 0;
    // virtual MirFormFactor getFormFactor() = 0;
    virtual uint8_t const* getEdid() = 0;
    // virtual char const* typeName(MirOutputType type) = 0;
    virtual bool isGammaSupported() = 0;
    virtual uint32_t getGammaSize() = 0;
    virtual void getGamma(uint16_t* red,
                          uint16_t* green,
                          uint16_t* blue,
                          uint32_t  size) = 0;

    // virtual void setCurrentMode(OutputMode const* mode) = 0;
    // void setPixelFormat(MirPixelFormat format);
    virtual void setPosition(int x, int y) = 0;

    virtual void enable() = 0;
    virtual void disable() = 0;

    virtual void setGamma(uint16_t const* red,
                          uint16_t const* green,
                          uint16_t const* blue,
                          uint32_t size) = 0;
    virtual void setPowerMode(Enums::PowerMode mode) = 0;
    virtual void setOrientation(Enums::Orientation orientation) = 0;
    virtual void setScaleFactor(float scale) = 0;
};
} // DisplayPlugin

#endif // DISPLAYS_OUTPUT_H
