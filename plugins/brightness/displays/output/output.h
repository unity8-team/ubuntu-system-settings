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

#include <QSharedPointer>

namespace DisplayPlugin
{
/* Provides an abstraction of a video output, which can be a physical or
virtual video output (hdmi, screencast, etc.). */
class Q_DECL_EXPORT Output
{
public:
    virtual ~Output() {};

    virtual QSharedPointer<OutputMode> getPreferredMode() = 0;
    virtual QSharedPointer<OutputMode> getCurrentMode() = 0;
    virtual QList<QSharedPointer<OutputMode>> getAvailableModes() = 0;
    virtual int getId() = 0;
    virtual Enums::OutputType getType() = 0;
    virtual int getPositionX() = 0;
    virtual int getPositionY() = 0;
    virtual Enums::ConnectionState getConnectionState() = 0;
    virtual bool isEnabled() = 0;
    virtual QString getModel() = 0;
    virtual int getPhysicalWidthMm() = 0;
    virtual int getPhysicalHeightMm() = 0;
    virtual Enums::PowerMode getPowerMode() = 0;
    virtual Enums::Orientation getOrientation() = 0;
    virtual float getScaleFactor() = 0;
    virtual uint8_t const* getEdid() = 0;

    virtual void setCurrentMode(const QSharedPointer<OutputMode> &mode) = 0;
    virtual void setPosition(const int &x, const int &y) = 0;
    virtual void setEnabled(const bool enabled) = 0;
    virtual void setPowerMode(const Enums::PowerMode &mode) = 0;
    virtual void setOrientation(const Enums::Orientation &orientation) = 0;
    virtual void setScaleFactor(const float &scale) = 0;
};
} // DisplayPlugin

Q_DECLARE_METATYPE(QSharedPointer<DisplayPlugin::OutputMode>)

#endif // DISPLAYS_OUTPUT_H
