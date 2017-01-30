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

#ifndef FAKE_OUTPUT_H
#define FAKE_OUTPUT_H

#include "outputmode/outputmode.h"

namespace DisplayPlugin
{
class FakeOutput : public Output
{
public:
    explicit FakeOutput() {}
    ~FakeOutput() {}
    virtual int getNumModes() override
    {

    }
    virtual size_t getPreferredModeIndex() override
    {

    }
    virtual size_t getCurrentModeIndex() override
    {

    }
    virtual int getNumPixelFormats() override
    {

    }
    virtual int getId() override
    {

    }
    virtual int getPositionX() override
    {

    }
    virtual int getPositionY() override
    {

    }
    virtual Enums::ConnectionState getConnectionState() override
    {

    }
    virtual bool is_enabled() override
    {

    }
    virtual char const* getModel() override
    {

    }
    virtual int getPhysicalWidthMm() override
    {

    }
    virtual int getPhysicalHeightMm() override
    {

    }
    virtual Enums::PowerMode getPowerMode() override
    {

    }
    virtual Enums::Orientation getOrientation() override
    {

    }
    virtual float getScaleFactor() override
    {

    }
    virtual uint8_t const* getEdid() override
    {

    }
    virtual bool isGammaSupported() override
    {

    }
    virtual uint32_t getGammaSize() override
    {

    }
    virtual void getGamma(uint16_t* red,
                          uint16_t* green,
                          uint16_t* blue,
                          uint32_t  size) override
    {

    }


    virtual void setPosition(int x, int y) override
    {

    }
    virtual void enable() override
    {

    }
    virtual void disable() override
    {

    }
    virtual void setGamma(uint16_t const* red,
                          uint16_t const* green,
                          uint16_t const* blue,
                          uint32_t  size) override
    {

    }
    virtual void setPowerMode(Enums::PowerMode mode) override
    {

    }
    virtual void setOrientation(Enums::Orientation orientation) override
    {

    }
    virtual void setScaleFactor(float scale) override
    {

    }
};
} // DisplaPlugin

#endif // FAKE_OUTPUT_H
