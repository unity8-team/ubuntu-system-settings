/*
 * Copyright (C) 2015 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
 */

#include <QDebug>

#include "display.h"

Display::Display(MirDisplayOutput *output) {
    setDisplayOutput(output);
}

void Display::updateModes() {
    for (unsigned int i = 0; i < m_mirOutput->num_modes; ++i) {
        MirDisplayMode mode = m_mirOutput->modes[i];
        m_availableModes <<  QString("%1x%2x%3").arg(
                QString::number(mode.vertical_resolution),
                QString::number(mode.horizontal_resolution),
                QString::number(mode.refresh_rate));
        if (i == m_mirOutput->current_mode) {
            m_currentMode = i;
            m_refreshRate = mode.refresh_rate;
        }
    }
}

void Display::updateSizes() {
    m_physicalSize = QSizeF(
        QSize(m_mirOutput->physical_width_mm,
              m_mirOutput->physical_height_mm)
    );
}

void Display::updateOrientation() {
    m_orientation = m_mirOutput->orientation;
}

int Display::id() const {
    return m_id;
}
bool Display::enabled() const {
    return m_enabled && m_powerMode == mir_power_mode_on;
}

bool Display::connected() const {
    return m_connected;
}

QString Display::mode() const {
    return m_availableModes.value(m_currentMode, QString());
}

QStringList Display::availableModes() const {
    return m_availableModes;
}

Display::Orientation Display::orientation() const {
    switch (m_orientation) {
        case mir_orientation_normal:
            return Display::Orientation::Normal;
        case mir_orientation_left:
            return Display::Orientation::PortraitMode;
        case mir_orientation_inverted:
            return Display::Orientation::LandscapeInvertedMode;
        case mir_orientation_right:
            return Display::Orientation::PortraitInvertedMode;
        default:
            return Display::Orientation::Normal;
    }
}

MirDisplayOutput * Display::getDisplayOutput() const {
    return m_mirOutput;
}

void Display::setEnabled(const bool &enabled) {
    m_mirOutput->used = enabled;
    if (enabled) {
        int index = m_availableModes.indexOf(mode());
        uint32_t i = (uint32_t)index;
        m_mirOutput->current_mode = i;
        m_mirOutput->power_mode = mir_power_mode_on;
    } else {
        m_mirOutput->current_mode = 0;
        m_mirOutput->power_mode = mir_power_mode_standby;
    }
    m_powerMode = m_mirOutput->power_mode;
}

void Display::setMode(const QString &mode) {
    int index = m_availableModes.indexOf(mode);
    if (index >= 0) {
        uint32_t i = (uint32_t)index;
        m_mirOutput->current_mode = i;
    } else {
        qWarning() << __PRETTY_FUNCTION__ << "failed to set mode to" << mode;
    }
}

void Display::setOrientation(const Orientation &orientation) {
    MirOrientation newOrientation;
    switch (orientation) {
        case Display::Orientation::Normal:
            newOrientation = mir_orientation_normal;
            break;
        case Display::Orientation::PortraitMode:
            newOrientation = mir_orientation_left;
            break;
        case Display::Orientation::LandscapeInvertedMode:
            newOrientation = mir_orientation_inverted;
            break;
        case Display::Orientation::PortraitInvertedMode:
            newOrientation = mir_orientation_right;
            break;
        default:
            newOrientation = mir_orientation_normal;
    }
    m_orientation = newOrientation;
}

void Display::setDisplayOutput(MirDisplayOutput * output) {
    m_mirOutput = output;
    if (output) {
        m_connected = output->connected;
        m_enabled = output->used && output->power_mode == mir_power_mode_on;
        m_id = output->output_id;
        m_powerMode = static_cast<MirPowerMode>(output->power_mode);
        m_orientation = static_cast<MirOrientation>(output->orientation);

        updateModes();
        updateOrientation();
        updateSizes();
    }
}
