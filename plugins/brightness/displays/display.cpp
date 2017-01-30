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

#include "display.h"
#include "helpers.h"

#include <QDebug>

namespace DisplayPlugin
{
QString DisplayMode::toString() const
{
    /* TRANSLATORS: %1 refer to the amount of horizontal pixels in a
    display resolution, and %2 to the vertical pixels. E.g. 1200x720.
    %3 is the refresh rate in hz. */
    return SystemSettings::_("%1×%2 @ %3hz")
        .arg(horizontal_resolution)
        .arg(vertical_resolution)
        .arg(refresh_rate);
}
bool DisplayMode::operator==(const DisplayMode &other) const
{
    return (
        horizontal_resolution == other.horizontal_resolution
        && vertical_resolution == other.vertical_resolution
        && refresh_rate == other.refresh_rate
    );
}


Display::Display(const uint &id)
{
    m_id = id;
    initialize();
}

Display::Display(QObject *parent)
    : QObject(parent)
{
    initialize();
}

Display::Display(QSharedPointer<Output> output, QObject *parent)
    : Display(parent)
{
    // m_type = DisplayPlugin::Helpers::mirTypeToString(output.type);
    // setConnected(output.connected);
    // setEnabled(output.used);

    // auto modes = QList<DisplayMode>();
    // for(uint j = 0; j < output.num_modes; j++) {
    //     DisplayMode mode(output.modes[j]);
    //     modes.append(mode);

    //     if (j == output.current_mode)
    //         m_mode = mode;
    // }
    // m_modes = modes;
    // m_orientation = DisplayPlugin::Helpers::mirOrientationToOrientation(output.orientation);
    // m_powerMode = DisplayPlugin::Helpers::mirPowerModeToPowerMode(output.power_mode);
    // m_id = output.output_id;

    // m_physicalWidthMm = output.physical_width_mm;
    // m_physicalHeightMm = output.physical_height_mm;
    // m_name = QString("%1").arg(DisplayPlugin::Helpers::mirTypeToString(output.type));

    // storeConfiguration();
    // changedSlot();
}

void Display::initialize()
{
    QObject::connect(this, SIGNAL(mirroredChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(connectedChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(enabledChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(modeChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(modesChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(orientationChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(scaleChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(uncommittedChangesChanged()),
                     this, SLOT(changedSlot()));
}

void Display::storeConfiguration()
{
    m_storedConfig["name"] = QVariant(m_name);
    m_storedConfig["type"] = QVariant(m_type);
    m_storedConfig["mirrored"] = QVariant(m_mirrored);
    m_storedConfig["enabled"] = QVariant(m_enabled);
    m_storedConfig["mode"] = QVariant(mode());
    m_storedConfig["orientation"] = QVariant::fromValue(m_orientation);
    m_storedConfig["scale"] = QVariant(m_scale);
}

bool Display::hasChanged() const
{
    return (
        m_storedConfig["name"].toString() != m_name
        || m_storedConfig["type"].toString() != m_type
        || m_storedConfig["mirrored"].toBool() != m_mirrored
        || m_storedConfig["enabled"].toBool() != m_enabled
        || m_storedConfig["mode"].toUInt() != mode()
        || m_storedConfig["orientation"].value<Enums::Orientation>() != m_orientation
        || m_storedConfig["scale"].toUInt() != m_scale
    );
}

uint Display::id() const
{
    return m_id;
}

QString Display::name() const
{
    return m_name;
}

QString Display::type() const
{
    return m_type;
}

bool Display::mirrored() const
{
    return m_mirrored;
}

bool Display::connected() const
{
    return m_connected;
}

bool Display::enabled() const
{
    return m_enabled;
}

uint Display::mode() const
{
    return m_modes.empty() ? 0 : m_modes.indexOf(m_mode);
}

QStringList Display::modes() const
{
    QStringList modes;
    Q_FOREACH(const DisplayMode &mode, m_modes) {
        modes.append(mode.toString());
    }
    return modes;
}

QList<DisplayMode> Display::availableModes() const
{
    return m_modes;
}

Enums::Orientation Display::orientation() const
{
    return m_orientation;
}

double Display::scale() const
{
    return m_scale;
}

bool Display::uncommittedChanges() const
{
    return m_uncommittedChanges;
}

uint Display::physicalWidthMm() const
{
    return m_physicalWidthMm;
}

uint Display::physicalHeightMm() const
{
    return m_physicalHeightMm;
}

Enums::PowerMode Display::powerMode() const
{
    return m_powerMode;
}

void Display::setMirrored(const bool &mirrored)
{
    if (m_mirrored != mirrored) {
        m_mirrored = mirrored;
        Q_EMIT mirroredChanged();
    }
}

void Display::setConnected(const bool &connected)
{
    if (m_connected != connected) {
        m_connected = connected;
        Q_EMIT connectedChanged();
    }
}

void Display::setEnabled(const bool &enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged();
    }
}

void Display::setMode(const uint &mode)
{
    m_mode = m_modes.value(mode);
    Q_EMIT modeChanged();
}

void Display::setOrientation(const Enums::Orientation &orientation)
{
    if (m_orientation != orientation) {
        m_orientation = orientation;
        Q_EMIT orientationChanged();
    }
}

void Display::setScale(const double &scale)
{
    if (m_scale != scale) {
        m_scale = scale;
        Q_EMIT scaleChanged();
    }
}

void Display::setUncommitedChanges(const bool uncommittedChanges)
{
    if (m_uncommittedChanges != uncommittedChanges) {
        m_uncommittedChanges = uncommittedChanges;
        Q_EMIT uncommittedChangesChanged();
    }
}

void Display::changedSlot()
{
    setUncommitedChanges(hasChanged());
    Q_EMIT displayChanged(this);
}
} // DisplayPlugin
