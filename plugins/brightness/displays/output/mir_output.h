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
    virtual void setCurrentMode(const QSharedPointer<OutputMode> &mode) override;
    virtual void setPosition(const int &x, const int &y) override;
    virtual void setEnabled(const bool enabled) override;
    virtual void setPowerMode(const Enums::PowerMode &mode) override;
    virtual void setOrientation(const Enums::Orientation &orientation) override;
    virtual void setScaleFactor(const float &scale) override;

private:
    MirOutput *m_output = Q_NULLPTR;
};
} // DisplayPlugin

#endif // DISPLAYS_MIR_OUTPUT_H
