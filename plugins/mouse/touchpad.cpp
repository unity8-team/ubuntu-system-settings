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

#include "touchpad.h"

#include <QDebug>

TouchPad::TouchPad(QObject *parent) :
    Mouse(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_unityInterface ("com.canonical.unity",
                   "/com/canonical/unity",
                   "com.canonical.unity",
                   m_systemBusConnection),
    m_enabled(false),
    m_connected(false)
{
    m_primaryButton = Mouse::Button::Left;
}

TouchPad::~TouchPad()
{
}

bool TouchPad::disableWhileTyping() const
{
    return m_disableWhileTyping;
}

bool TouchPad::tapToClick() const
{
    return m_tapToClick;
}

bool TouchPad::twoFingerScroll() const
{
    return m_twoFingerScroll;
}

void TouchPad::setDisableWhileTyping(const bool &enabled)
{
    qWarning() << Q_FUNC_INFO << enabled;
    m_disableWhileTyping = enabled;
    Q_EMIT disableWhileTypingChanged(enabled);
}

void TouchPad::setTapToClick(const bool &enabled)
{
    qWarning() << Q_FUNC_INFO << enabled;
    m_tapToClick = enabled;
    Q_EMIT tapToClickChanged(enabled);
}

void TouchPad::setTwoFingerScroll(const bool &enabled)
{
    qWarning() << Q_FUNC_INFO << enabled;
    m_twoFingerScroll = enabled;
    Q_EMIT twoFingerScrollChanged(enabled);
}
