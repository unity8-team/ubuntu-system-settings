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

#ifndef DISPLAYS_MIR_OUTPUT_H
#define DISPLAYS_MIR_OUTPUT_H

#include "output.h"

#include <mir_toolkit/mir_client_library.h>

namespace DisplayPlugin
{
class MirOutputImpl : public Output
{
public:
    explicit MirOutputImpl(MirOutput *output);
    virtual ~MirOutputImpl();

    virtual QSharedPointer<OutputMode> getPreferredMode() override;
    virtual QSharedPointer<OutputMode> getCurrentMode() override;
    virtual QList<QSharedPointer<OutputMode>> getAvailableModes() override;
    virtual int getNumPixelFormats() override;
    virtual int getId() override;
    virtual Enums::OutputType getType() override;
    virtual int getPositionX() override;
    virtual int getPositionY() override;
    virtual Enums::ConnectionState getConnectionState() override;
    virtual bool isEnabled() override;
    virtual QString getModel() override;
    virtual int getPhysicalWidthMm() override;
    virtual int getPhysicalHeightMm() override;
    virtual Enums::PowerMode getPowerMode() override;
    virtual Enums::Orientation getOrientation() override;
    virtual float getScaleFactor() override;
    virtual uint8_t const* getEdid() override;
    // virtual char const* typeName(MirOutputType type) override;
    virtual bool isGammaSupported() override;
    virtual uint32_t getGammaSize() override;
    virtual void getGamma(uint16_t* red,
                          uint16_t* green,
                          uint16_t* blue,
                          uint32_t  size) override;

    virtual void setCurrentMode(const QSharedPointer<OutputMode> &mode) override;
    virtual void setPosition(int x, int y) override;
    virtual void enable() override;
    virtual void disable() override;
    virtual void setGamma(uint16_t const* red,
                          uint16_t const* green,
                          uint16_t const* blue,
                          uint32_t size) override;
    virtual void setPowerMode(Enums::PowerMode mode) override;
    virtual void setOrientation(Enums::Orientation orientation) override;
    virtual void setScaleFactor(float scale) override;

private:
    MirOutput *m_output = Q_NULLPTR;
};
} // DisplayPlugin

#endif // DISPLAYS_MIR_OUTPUT_H
