/*
 * Copyright (C) 2015-2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QTMIR_MIRSURFACEINTERFACE_H
#define QTMIR_MIRSURFACEINTERFACE_H

// Unity API
#include <unity/shell/application/MirSurfaceInterface.h>

#include "session_interface.h"

// Qt
#include <QCursor>
#include <QSharedPointer>
#include <QTouchEvent>

class QHoverEvent;
class QMouseEvent;
class QKeyEvent;
class QSGTexture;

namespace qtmir {

class MirSurfaceInterface : public unity::shell::application::MirSurfaceInterface
{
    Q_OBJECT

public:
    MirSurfaceInterface(QObject *parent = nullptr) : unity::shell::application::MirSurfaceInterface(parent) {}
    virtual ~MirSurfaceInterface() {}

    virtual void setLive(bool value) = 0;

    virtual bool isFirstFrameDrawn() const = 0;

    virtual void stopFrameDropper() = 0;
    virtual void startFrameDropper() = 0;

    virtual bool isBeingDisplayed() const = 0;

    virtual void registerView(qintptr viewId) = 0;
    virtual void unregisterView(qintptr viewId) = 0;
    virtual void setViewVisibility(qintptr viewId, bool visible) = 0;

    // methods called from the rendering (scene graph) thread:
    virtual QSharedPointer<QSGTexture> texture() = 0;
    virtual QSGTexture *weakTexture() const = 0;
    virtual bool updateTexture() = 0;
    virtual unsigned int currentFrameNumber() const = 0;
    virtual bool numBuffersReadyForCompositor() = 0;
    // end of methods called from the rendering (scene graph) thread

    /*
        Defines the unityapi::MirSurfaceInterface::focused() value, which is what shell sees.
        Set centrally by MirFocusController and used for window-management purposes by shell.
     */
    virtual void setFocused(bool focus) = 0;

    /*
        Defines the "focus" attribute of the underlying mir::scene::Surface, which is what
        the client application sees.
        Set by MirSurfaceItems (aka views) and used to inform the client application about the
        actual focus, pretty much a one-to-one mapping with QML's active focus concept, hence
        the name.
     */
    virtual void setViewActiveFocus(qintptr viewId, bool value) = 0;
    /*
        Whether any view of this surface currently has QML active focus
     */
    virtual bool activeFocus() const = 0;

    virtual void mousePressEvent(QMouseEvent *event) = 0;
    virtual void mouseMoveEvent(QMouseEvent *event) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event) = 0;
    virtual void hoverEnterEvent(QHoverEvent *event) = 0;
    virtual void hoverLeaveEvent(QHoverEvent *event) = 0;
    virtual void hoverMoveEvent(QHoverEvent *event) = 0;
    virtual void wheelEvent(QWheelEvent *event) = 0;

    virtual void keyPressEvent(QKeyEvent *event) = 0;
    virtual void keyReleaseEvent(QKeyEvent *event) = 0;

    virtual void touchEvent(Qt::KeyboardModifiers qmods,
            const QList<QTouchEvent::TouchPoint> &qtTouchPoints,
            Qt::TouchPointStates qtTouchPointStates,
            ulong qtTimestamp) = 0;

    virtual QString appId() const = 0;

    virtual QCursor cursor() const = 0;

    virtual void setScreen(QScreen *screen) = 0;

    virtual SessionInterface* session() = 0;

    virtual bool inputAreaContains(const QPoint &) const = 0;

public Q_SLOTS:
    virtual void onCompositorSwappedBuffers() = 0;

    virtual void setMinimumWidth(int) = 0;
    virtual void setMinimumHeight(int) = 0;
    virtual void setMaximumWidth(int) = 0;
    virtual void setMaximumHeight(int) = 0;
    virtual void setWidthIncrement(int) = 0;
    virtual void setHeightIncrement(int) = 0;
    virtual void setShellChrome(Mir::ShellChrome shellChrome) = 0;

Q_SIGNALS:
    void cursorChanged(const QCursor &cursor);
    void raiseRequested();
    void closeRequested();
    void firstFrameDrawn();
    void framesPosted();
    void isBeingDisplayedChanged();
    void frameDropped();
};

} // namespace qtmir

#endif // QTMIR_MIRSURFACEINTERFACE_H
