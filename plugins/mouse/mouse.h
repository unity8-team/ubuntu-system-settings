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
 * Authors: Ken VanDine <ken.vandine@canonical.com>
 */

#ifndef MOUSE_H
#define MOUSE_H

#include "accountsservice.h"

#include <QDBusServiceWatcher>
#include <QtCore/QObject>
#include <QtCore/QString>

class Mouse: public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString mousePrimaryButton
                READ getMousePrimaryButton
                WRITE setMousePrimaryButton
                NOTIFY mousePrimaryButtonChanged)
    Q_PROPERTY (double mouseCursorSpeed
                READ getMouseCursorSpeed
                WRITE setMouseCursorSpeed
                NOTIFY mouseCursorSpeedChanged)
    Q_PROPERTY (double mouseScrollSpeed
                READ getMouseScrollSpeed
                WRITE setMouseScrollSpeed
                NOTIFY mouseScrollSpeedChanged)
    Q_PROPERTY (int mouseDoubleClickSpeed
                READ getMouseDoubleClickSpeed
                WRITE setMouseDoubleClickSpeed
                NOTIFY mouseDoubleClickSpeedChanged)
    Q_PROPERTY (QString touchpadPrimaryButton
                READ getTouchpadPrimaryButton
                WRITE setTouchpadPrimaryButton
                NOTIFY touchpadPrimaryButtonChanged)
    Q_PROPERTY (double touchpadCursorSpeed
                READ getTouchpadCursorSpeed
                WRITE setTouchpadCursorSpeed
                NOTIFY touchpadCursorSpeedChanged)
    Q_PROPERTY (double touchpadScrollSpeed
                READ getTouchpadScrollSpeed
                WRITE setTouchpadScrollSpeed
                NOTIFY touchpadScrollSpeedChanged)
    Q_PROPERTY (int touchpadDoubleClickSpeed
                READ getTouchpadDoubleClickSpeed
                WRITE setTouchpadDoubleClickSpeed
                NOTIFY touchpadDoubleClickSpeedChanged)
    Q_PROPERTY (bool touchpadDisableWhileTyping
                READ getTouchpadDisableWhileTyping
                WRITE setTouchpadDisableWhileTyping
                NOTIFY touchpadDisableWhileTypingChanged)
    Q_PROPERTY (bool touchpadTapToClick
                READ getTouchpadTapToClick
                WRITE setTouchpadTapToClick
                NOTIFY touchpadTapToClickChanged)
    Q_PROPERTY (bool touchpadTwoFingerScroll
                READ getTouchpadTwoFingerScroll
                WRITE setTouchpadTwoFingerScroll
                NOTIFY touchpadTwoFingerScrollChanged)
    Q_PROPERTY (bool touchpadDisableWithMouse
                READ getTouchpadDisableWithMouse
                WRITE setTouchpadDisableWithMouse
                NOTIFY touchpadDisableWithMouseChanged)

public:
    explicit Mouse(QObject *parent = 0);
    virtual ~Mouse();

    const QString getMousePrimaryButton();
    void setMousePrimaryButton(const QString);
    double getMouseCursorSpeed();
    void setMouseCursorSpeed(double);
    double getMouseScrollSpeed();
    void setMouseScrollSpeed(double);
    int getMouseDoubleClickSpeed();
    void setMouseDoubleClickSpeed(int);
    const QString getTouchpadPrimaryButton();
    void setTouchpadPrimaryButton(const QString);
    double getTouchpadCursorSpeed();
    void setTouchpadCursorSpeed(double);
    double getTouchpadScrollSpeed();
    void setTouchpadScrollSpeed(double);
    int getTouchpadDoubleClickSpeed();
    void setTouchpadDoubleClickSpeed(int);
    bool getTouchpadDisableWhileTyping();
    void setTouchpadDisableWhileTyping(bool);
    bool getTouchpadTapToClick();
    void setTouchpadTapToClick(bool);
    bool getTouchpadTwoFingerScroll();
    void setTouchpadTwoFingerScroll(bool);
    bool getTouchpadDisableWithMouse();
    void setTouchpadDisableWithMouse(bool);


public Q_SLOTS:
    void slotChanged(QString, QString);
    void slotNameOwnerChanged();

Q_SIGNALS:
    void mousePrimaryButtonChanged();
    void mouseCursorSpeedChanged();
    void mouseScrollSpeedChanged();
    void mouseDoubleClickSpeedChanged();
    void touchpadPrimaryButtonChanged();
    void touchpadCursorSpeedChanged();
    void touchpadScrollSpeedChanged();
    void touchpadDoubleClickSpeedChanged();
    void touchpadDisableWhileTypingChanged();
    void touchpadTapToClickChanged();
    void touchpadTwoFingerScrollChanged();
    void touchpadDisableWithMouseChanged();

private:
    AccountsService m_accountsService;
};

#endif //MOUSE_H
