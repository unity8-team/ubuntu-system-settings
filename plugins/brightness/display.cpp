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

#include "display.h"

#include <QDebug>

Display::Display(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_unityInterface ("com.canonical.unity",
                   "/com/canonical/unity",
                   "com.canonical.unity",
                   m_systemBusConnection),
    m_enabled(false),
    m_connected(false)
{
    m_resolution = QString("1600x1200x60");
    m_availableResolutions = QStringList();
    m_availableResolutions << m_resolution << "1280x1024x60" << "1024x768x60";
    m_orientation = Display::OrientationMode::AnyMode;
}

Display::~Display()
{
}

QString Display::path() const
{
    return m_path;
}

bool Display::enabled() const
{
    return m_enabled;
}

QString Display::resolution() const
{
    return m_resolution;
}

QStringList Display::availableResolutions() const
{
    return m_availableResolutions;
}

Display::OrientationMode Display::orientation() const
{
    return m_orientation;
}

float Display::scale() const
{
    return m_scale;
}

bool Display::connected() const
{
    // return m_connected;
    return true;
}

void Display::setPath(const QString &path)
{
    qWarning() << "Sat path" << path;
    m_path = path;
    Q_EMIT pathChanged(path);
}

void Display::setEnabled(const bool &enabled)
{
    qWarning() << "Sat enabled" << enabled;
    m_enabled = enabled;
    Q_EMIT enabledChanged(enabled);
}

void Display::setOrientation(const Display::OrientationMode &orientation)
{
    qWarning() << "Sat orientation" << orientation;
    m_orientation = orientation;
    Q_EMIT orientationChanged(orientation);
}

void Display::setResolution(const QString &resolution)
{
    qWarning() << "Sat resolution" << resolution;
    m_resolution = resolution;
    Q_EMIT resolutionChanged(resolution);
}

void Display::setScale(const float &scale)
{
    qWarning() << "Sat scale" << scale;
    m_scale = scale;
    Q_EMIT scaleChanged(scale);
}
