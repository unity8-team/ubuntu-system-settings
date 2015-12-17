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
    m_mirOutput = output;
    m_connected = output->connected;
    m_enabled = output->used;
    m_name = QString("Display %1").arg(QString::number(output->output_id));
    qWarning() << "Display created" << output->type << output->connected << output->used;

    if (m_mirOutput) {
        updateModes();
        updateOrientation();
        updateSizes();
    }
}

void Display::updateModes() {
    qWarning() << "updateModes";
    for (unsigned int i = 0; i < m_mirOutput->num_modes; ++i) {
        MirDisplayMode mode = m_mirOutput->modes[i];
        m_availableModes <<  QString("%1x%2x%3").arg(
                QString::number(mode.vertical_resolution),
                QString::number(mode.horizontal_resolution),
                QString::number(mode.refresh_rate));
        qWarning() << "mode" << i << m_availableModes;
        if (i == m_mirOutput->current_mode) {
            m_currentMode = i;
            m_refreshRate = mode.refresh_rate;
        }
    }
}

void Display::updateSizes() {
    qWarning() << "updateSizes";
    m_physicalSize = QSizeF(
        QSize(m_mirOutput->physical_width_mm,
              m_mirOutput->physical_height_mm)
    );
}

void Display::updateOrientation() {
    qWarning() << "updateOrientation";
    m_orientation = m_mirOutput->orientation;
}

QString Display::name() const {
    return m_name;
}

bool Display::enabled() const {
    return m_enabled;
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
    return (Display::Orientation)m_orientation;
}

MirDisplayOutput * Display::output() const {
    return m_mirOutput;
}

void Display::setEnabled(const bool &enabled) {
    m_mirOutput->used = enabled;
    qWarning() << __PRETTY_FUNCTION__ << enabled;
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
    qWarning() << __PRETTY_FUNCTION__ << orientation;
}
