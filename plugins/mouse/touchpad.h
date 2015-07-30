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

#ifndef TOUCHPAD_H
#define TOUCHPAD_H

#include <QDBusInterface>
#include <QObject>
#include "mouse.h"

class TouchPad : public Mouse
{
    Q_OBJECT
    Q_PROPERTY( bool disableWhileTyping
                READ disableWhileTyping
                WRITE setDisableWhileTyping
                NOTIFY disableWhileTypingChanged )
    Q_PROPERTY( bool tapToClick
                READ tapToClick
                WRITE setTapToClick
                NOTIFY tapToClickChanged )
    Q_PROPERTY( bool twoFingerScroll
                READ twoFingerScroll
                WRITE setTwoFingerScroll
                NOTIFY twoFingerScrollChanged )

public:

    explicit TouchPad(QObject *parent = 0);
    ~TouchPad();

    bool disableWhileTyping() const;
    void setDisableWhileTyping(const bool &enabled);

    bool tapToClick() const;
    void setTapToClick(const bool &enabled);

    bool twoFingerScroll() const;
    void setTwoFingerScroll(const bool &enabled);

Q_SIGNALS:
    void disableWhileTypingChanged(bool connected);
    void tapToClickChanged(bool connected);
    void twoFingerScrollChanged(bool connected);

private:
    QDBusConnection m_systemBusConnection;
    QDBusInterface m_unityInterface;
    QString m_path;
    bool m_enabled;
    Button m_primaryButton;
    float m_cursorSpeed;
    float m_clickSpeed;
    float m_scrollSpeed;
    bool m_connected;
    bool m_disableWhileTyping = true;
    bool m_tapToClick = true;
    bool m_twoFingerScroll = true;

};

#endif // TOUCHPAD_H
