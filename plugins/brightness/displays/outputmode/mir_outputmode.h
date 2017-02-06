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

#ifndef DISPLAYS_MIR_OUTPUTMODE_H
#define DISPLAYS_MIR_OUTPUTMODE_H

#include "outputmode.h"

#include <mir_toolkit/mir_client_library.h>

namespace DisplayPlugin
{
class MirOutputModeImpl : public OutputMode
{
public:
    explicit MirOutputModeImpl(MirOutputMode const *outputMode);
    virtual ~MirOutputModeImpl();

    virtual int getWidth() override;
    virtual int getHeight() override;
    virtual double getRefreshRate() override;
    virtual QString toString() override;
    MirOutputMode const* getMirOutputMode();
private:
    MirOutputMode const *m_outputMode = Q_NULLPTR;
};
} // DisplayPlugin

#endif // DISPLAYS_MIR_OUTPUTMODE_H
