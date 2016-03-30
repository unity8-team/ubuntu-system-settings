/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Ken VanDine <ken.vandine@canonical.com>
 */

#include "mouse.h"

#define AS_INTERFACE "com.ubuntu.AccountsService.Input"

Mouse::Mouse(QObject* parent)
  : QObject(parent)
{
    connect (&m_accountsService,
             SIGNAL (propertyChanged (QString, QString)),
             this,
             SLOT (slotChanged (QString, QString)));

    connect (&m_accountsService,
             SIGNAL (nameOwnerChanged()),
             this,
             SLOT (slotNameOwnerChanged()));
}

Mouse::~Mouse()
{
}

void Mouse::slotChanged(QString interface,
                        QString property)
{
    if (interface == AS_INTERFACE) {
        if (property == "MousePrimaryButton") {
            Q_EMIT mousePrimaryButtonChanged();
        } else if (property == "MouseCursorSpeed") {
            Q_EMIT mouseCursorSpeedChanged();
        } else if (property == "MouseScrollSpeed") {
            Q_EMIT mouseScrollSpeedChanged();
        } else if (property == "MouseDoubleClickSpeed") {
            Q_EMIT mouseDoubleClickSpeedChanged();
        } else if (property == "TouchpadPrimaryButton") {
            Q_EMIT touchpadPrimaryButtonChanged();
        } else if (property == "TouchpadCursorSpeed") {
            Q_EMIT touchpadCursorSpeedChanged();
        } else if (property == "TouchpadScrollSpeed") {
            Q_EMIT touchpadScrollSpeedChanged();
        } else if (property == "TouchpadDoubleClickSpeed") {
            Q_EMIT touchpadDoubleClickSpeedChanged();
        } else if (property == "TouchpadDisableWhileTyping") {
            Q_EMIT touchpadDisableWhileTypingChanged();
        } else if (property == "TouchpadTapToClick") {
            Q_EMIT touchpadTapToClickChanged();
        } else if (property == "TouchpadTwoFingerScroll") {
            Q_EMIT touchpadTwoFingerScrollChanged();
        } else if (property == "TouchpadDisableWithMouse") {
            Q_EMIT touchpadDisableWithMouseChanged();
        }
    }
}

void Mouse::slotNameOwnerChanged()
{
    // Tell QML so that it refreshes its view of the property
    Q_EMIT mousePrimaryButtonChanged();
    Q_EMIT mouseCursorSpeedChanged();
    Q_EMIT mouseScrollSpeedChanged();
    Q_EMIT mouseDoubleClickSpeedChanged();
    Q_EMIT touchpadPrimaryButtonChanged();
    Q_EMIT touchpadCursorSpeedChanged();
    Q_EMIT touchpadScrollSpeedChanged();
    Q_EMIT touchpadDoubleClickSpeedChanged();
    Q_EMIT touchpadDisableWhileTypingChanged();
    Q_EMIT touchpadTapToClickChanged();
    Q_EMIT touchpadTwoFingerScrollChanged();
    Q_EMIT touchpadDisableWithMouseChanged();
}

const QString Mouse::getMousePrimaryButton ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "MousePrimaryButton").toString();
}

void Mouse::setMousePrimaryButton (const QString primary)
{
    if (primary == getMousePrimaryButton())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "MousePrimaryButton",
                                      QVariant::fromValue(primary));
    Q_EMIT (mousePrimaryButtonChanged());
}

double Mouse::getMouseCursorSpeed ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "MouseCursorSpeed").toDouble();
}

void Mouse::setMouseCursorSpeed (double speed)
{
    if (speed == getMouseCursorSpeed())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "MouseCursorSpeed",
                                      QVariant::fromValue(speed));
    Q_EMIT (mouseCursorSpeedChanged());
}

double Mouse::getMouseScrollSpeed ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "MouseScrollSpeed").toDouble();
}

void Mouse::setMouseScrollSpeed (double speed)
{
    if (speed == getMouseScrollSpeed())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "MouseScrollSpeed",
                                      QVariant::fromValue(speed));
    Q_EMIT (mouseScrollSpeedChanged());
}

int Mouse::getMouseDoubleClickSpeed ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "MouseDoubleClickSpeed").toInt();
}

void Mouse::setMouseDoubleClickSpeed (int speed)
{
    if (speed == getMouseDoubleClickSpeed())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "MouseDoubleClickSpeed",
                                      QVariant::fromValue(speed));
    Q_EMIT (mouseDoubleClickSpeedChanged());
}

const QString Mouse::getTouchpadPrimaryButton ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "TouchpadPrimaryButton").toString();
}

void Mouse::setTouchpadPrimaryButton (const QString primary)
{
    if (primary == getTouchpadPrimaryButton())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "TouchpadPrimaryButton",
                                      QVariant::fromValue(primary));
    Q_EMIT (touchpadPrimaryButtonChanged());
}

double Mouse::getTouchpadCursorSpeed ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "TouchpadCursorSpeed").toDouble();
}

void Mouse::setTouchpadCursorSpeed (double speed)
{
    if (speed == getTouchpadCursorSpeed())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "TouchpadCursorSpeed",
                                      QVariant::fromValue(speed));
    Q_EMIT (touchpadCursorSpeedChanged());
}

double Mouse::getTouchpadScrollSpeed ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "TouchpadScrollSpeed").toDouble();
}

void Mouse::setTouchpadScrollSpeed (double speed)
{
    if (speed == getTouchpadScrollSpeed())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "TouchpadScrollSpeed",
                                      QVariant::fromValue(speed));
    Q_EMIT (touchpadScrollSpeedChanged());
}

int Mouse::getTouchpadDoubleClickSpeed ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "TouchpadDoubleClickSpeed").toInt();
}

void Mouse::setTouchpadDoubleClickSpeed (int speed)
{
    if (speed == getTouchpadDoubleClickSpeed())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "TouchpadDoubleClickSpeed",
                                      QVariant::fromValue(speed));
    Q_EMIT (touchpadDoubleClickSpeedChanged());
}

bool Mouse::getTouchpadDisableWhileTyping ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "TouchpadDisableWhileTyping").toBool();
}

void Mouse::setTouchpadDisableWhileTyping (bool enabled)
{
    if (enabled == getTouchpadDisableWhileTyping())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "TouchpadDisableWhileTyping",
                                      QVariant::fromValue(enabled));
    Q_EMIT (touchpadDisableWhileTypingChanged());
}

bool Mouse::getTouchpadTapToClick ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "TouchpadTapToClick").toBool();
}

void Mouse::setTouchpadTapToClick (bool enabled)
{
    if (enabled == getTouchpadTapToClick())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "TouchpadTapToClick",
                                      QVariant::fromValue(enabled));
    Q_EMIT (touchpadTapToClickChanged());
}

bool Mouse::getTouchpadTwoFingerScroll ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "TouchpadTwoFingerScroll").toBool();
}

void Mouse::setTouchpadTwoFingerScroll (bool enabled)
{
    if (enabled == getTouchpadTwoFingerScroll())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "TouchpadTwoFingerScroll",
                                      QVariant::fromValue(enabled));
    Q_EMIT (touchpadTwoFingerScrollChanged());
}

bool Mouse::getTouchpadDisableWithMouse ()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "TouchpadDisableWithMouse").toBool();
}

void Mouse::setTouchpadDisableWithMouse (bool enabled)
{
    if (enabled == getTouchpadDisableWithMouse())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "TouchpadDisableWithMouse",
                                      QVariant::fromValue(enabled));
    Q_EMIT (touchpadDisableWithMouseChanged());
}
