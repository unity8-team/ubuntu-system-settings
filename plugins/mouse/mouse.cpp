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
 * Ken VanDine <ken.vandine@canonical.com>
 *
 */

#include "mouse.h"

#include <QDebug>

Mouse::Mouse(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_unityInterface ("com.canonical.unity",
                   "/com/canonical/unity",
                   "com.canonical.unity",
                   m_systemBusConnection),
    m_enabled(false),
    m_connected(false)
{
    //m_primaryButton = Mouse::Button::Left;
}

Mouse::~Mouse()
{
}

QString Mouse::path() const
{
    return m_path;
}

bool Mouse::enabled() const
{
    return m_enabled;
}

Mouse::Button Mouse::primaryButton() const
{
    return m_primaryButton;
}

float Mouse::cursorSpeed() const
{
    return m_cursorSpeed;
}

float Mouse::clickSpeed() const
{
    return m_clickSpeed;
}

float Mouse::scrollSpeed() const
{
    return m_scrollSpeed;
}

bool Mouse::connected() const
{
    return m_connected;
}

void Mouse::setPath(const QString &path)
{
    qWarning() << Q_FUNC_INFO << path;
    m_path = path;
    Q_EMIT pathChanged(path);
}

void Mouse::setEnabled(const bool &enabled)
{
    qWarning() << Q_FUNC_INFO << enabled;
    m_enabled = enabled;
    Q_EMIT enabledChanged(enabled);
}

void Mouse::setPrimaryButton(const Mouse::Button &button)
{
    qWarning() << Q_FUNC_INFO << button;
    m_primaryButton = button;
    Q_EMIT primaryButtonChanged(button);
}

void Mouse::setCursorSpeed(const float &speed)
{
    qWarning() << Q_FUNC_INFO << speed;
    m_cursorSpeed = speed;
    Q_EMIT cursorSpeedChanged(speed);
}

void Mouse::setClickSpeed(const float &speed)
{
    qWarning() << Q_FUNC_INFO << speed;
    m_clickSpeed = speed;
    Q_EMIT clickSpeedChanged(speed);
}

void Mouse::setScrollSpeed(const float &speed)
{
    qWarning() << Q_FUNC_INFO << speed;
    m_scrollSpeed = speed;
    Q_EMIT scrollSpeedChanged(speed);
}
