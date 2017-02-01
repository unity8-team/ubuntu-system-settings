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

#include "output/output.h"

namespace DisplayPlugin
{
class FakeOutput : public Output
{
public:
    explicit FakeOutput() {}
    ~FakeOutput() {}
    virtual QSharedPointer<OutputMode> getPreferredMode() override
    {
        if (m_preferredModeIndex < m_modes.size()) {
            return m_modes.at(m_preferredModeIndex);
        } else {
            return QSharedPointer<OutputMode>(Q_NULLPTR);
        }
    }
    virtual QSharedPointer<OutputMode> getCurrentMode() override
    {
        if (m_currentModeIndex < m_modes.size()) {
            return m_modes.at(m_currentModeIndex);
        } else {
            return QSharedPointer<OutputMode>(Q_NULLPTR);
        }
    }
    virtual QList<QSharedPointer<OutputMode>> getAvailableModes() override
    {
        return m_modes;
    }
    virtual int getId() override
    {
        return m_id;
    }
    virtual Enums::OutputType getType() override
    {
        return m_type;
    }
    virtual int getPositionX() override
    {
        return m_positionX;
    }
    virtual int getPositionY() override
    {
        return m_positionY;
    }
    virtual Enums::ConnectionState getConnectionState() override
    {
        return m_connectionState;
    }
    virtual bool isEnabled() override
    {
        return m_enabled;
    }
    virtual QString getModel() override
    {
        return m_model;
    }
    virtual int getPhysicalWidthMm() override
    {
        return m_physicalWidthMm;
    }
    virtual int getPhysicalHeightMm() override
    {
        return m_physicalHeightMm;
    }
    virtual Enums::PowerMode getPowerMode() override
    {
        return m_powerMode;
    }
    virtual Enums::Orientation getOrientation() override
    {
        return m_orientation;
    }
    virtual float getScaleFactor() override
    {
        return m_scaleFactor;
    }
    virtual uint8_t const* getEdid() override
    {
        return m_edid;
    }
    virtual void setPosition(const int &x, const int &y) override
    {
        m_positionY = y;
        m_positionX = x;
    }
    virtual void setEnabled(const bool enabled) override
    {
        m_enabled = enabled;
    }
    virtual void setPowerMode(const Enums::PowerMode &mode) override
    {
        m_powerMode = mode;
    }
    virtual void setOrientation(const Enums::Orientation &orientation) override
    {
        m_orientation = orientation;
    }
    virtual void setScaleFactor(const float &scale) override
    {
        m_scaleFactor = scale;
    }
    virtual void setCurrentMode(const QSharedPointer<OutputMode> &mode) override
    {
        m_currentModeIndex = m_modes.indexOf(mode);
    }
    QList<QSharedPointer<OutputMode>> m_modes;
    bool m_enabled = false;
    bool m_gammaSupported = false;
    int m_numModes = 0;
    int m_preferredModeIndex;
    int m_currentModeIndex = 0;
    int m_numPixelFormats = 0;
    int m_id = 0;
    Enums::OutputType m_type = Enums::OutputType::OutputTypeUnknown;
    int m_positionX = 0;
    int m_positionY = 0;
    Enums::ConnectionState m_connectionState = Enums::ConnectionState::Unknown;
    QString m_model = QString::null;
    int m_physicalWidthMm = 0;
    int m_physicalHeightMm = 0;
    Enums::PowerMode m_powerMode = Enums::PowerMode::OffMode;
    Enums::Orientation m_orientation = Enums::Orientation::NormalOrientation;
    float m_scaleFactor = 0.0;
    uint8_t const* m_edid;
    uint32_t m_gammaSize = 0;
};
} // DisplaPlugin

#endif // FAKE_OUTPUT_H
