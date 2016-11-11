/*
 * Copyright (C) 2013-2016 Canonical, Ltd.
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

// local
#include "application.h"
#include "session.h"
#include "mirsurfaceitem.h"
#include "mirfocuscontroller.h"
#include "logging.h"
#include "tracepoints.h" // generated from tracepoints.tp
#include "timestamp.h"

// common
#include <debughelpers.h>

// Qt
#include <QDebug>
#include <QGuiApplication>
#include <QMutexLocker>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QScreen>
#include <private/qsgdefaultimagenode_p.h>
#include <QTimer>
#include <QSGTextureProvider>

#include <QRunnable>

namespace qtmir {

namespace {

class MirSurfaceItemReleaseResourcesJob : public QRunnable
{
public:
    MirSurfaceItemReleaseResourcesJob() : textureProvider(nullptr) {}
    void run() {
        delete textureProvider;
        textureProvider = nullptr;
    }
    QObject *textureProvider;
};

} // namespace {

class MirTextureProvider : public QSGTextureProvider
{
    Q_OBJECT
public:
    MirTextureProvider(const QSharedPointer<QSGTexture>& texture) : t(texture) {}
    QSGTexture *texture() const {
        if (t)
            t->setFiltering(smooth ? QSGTexture::Linear : QSGTexture::Nearest);
        return t.data();
    }

    bool smooth{false};

    void releaseTexture() {
        t.reset();
    }

    void setTexture(const QSharedPointer<QSGTexture>& newTexture) {
        t = newTexture;
    }

private:
    QSharedPointer<QSGTexture> t;
};

MirSurfaceItem::MirSurfaceItem(QQuickItem *parent)
    : MirSurfaceItemInterface(parent)
    , m_surface(nullptr)
    , m_window(nullptr)
    , m_textureProvider(nullptr)
    , m_lastTouchEvent(nullptr)
    , m_lastFrameNumberRendered(nullptr)
    , m_surfaceWidth(0)
    , m_surfaceHeight(0)
    , m_orientationAngle(nullptr)
    , m_consumesInput(false)
    , m_fillMode(Stretch)
{
    qCDebug(QTMIR_SURFACES) << "MirSurfaceItem::MirSurfaceItem";

    setSmooth(true);
    setFlag(QQuickItem::ItemHasContents, true); //so scene graph will render this item

    m_updateMirSurfaceSizeTimer.setSingleShot(true);
    m_updateMirSurfaceSizeTimer.setInterval(1);
    connect(&m_updateMirSurfaceSizeTimer, &QTimer::timeout, this, &MirSurfaceItem::updateMirSurfaceSize);

    connect(this, &QQuickItem::activeFocusChanged, this, &MirSurfaceItem::updateMirSurfaceActiveFocus);
    connect(this, &QQuickItem::visibleChanged, this, &MirSurfaceItem::updateMirSurfaceVisibility);
    connect(this, &QQuickItem::windowChanged, this, &MirSurfaceItem::onWindowChanged);
}

void MirSurfaceItem::componentComplete()
{
    QQuickItem::componentComplete();
    if (window()) {
        updateScreen(window()->screen());
    }
}

MirSurfaceItem::~MirSurfaceItem()
{
    qCDebug(QTMIR_SURFACES) << "MirSurfaceItem::~MirSurfaceItem - this=" << this;

    setSurface(nullptr);

    delete m_lastTouchEvent;
    delete m_lastFrameNumberRendered;
    delete m_orientationAngle;

    // Belongs to the scene graph thread. Can't delete here.
    // Scene graph should call MirSurfaceItem::releaseResources() or invalidateSceneGraph()
    // delete m_textureProvider;
}

Mir::Type MirSurfaceItem::type() const
{
    if (m_surface) {
        return m_surface->type();
    } else {
        return Mir::UnknownType;
    }
}

Mir::OrientationAngle MirSurfaceItem::orientationAngle() const
{
    if (m_orientationAngle) {
        Q_ASSERT(!m_surface);
        return *m_orientationAngle;
    } else if (m_surface) {
        return m_surface->orientationAngle();
    } else {
        return Mir::Angle0;
    }
}

void MirSurfaceItem::setOrientationAngle(Mir::OrientationAngle angle)
{
    qCDebug(QTMIR_SURFACES, "MirSurfaceItem::setOrientationAngle(%d)", angle);

    if (m_surface) {
        Q_ASSERT(!m_orientationAngle);
        m_surface->setOrientationAngle(angle);
    } else if (!m_orientationAngle) {
        m_orientationAngle = new Mir::OrientationAngle;
        *m_orientationAngle = angle;
        Q_EMIT orientationAngleChanged(angle);
    } else if (*m_orientationAngle != angle) {
        *m_orientationAngle = angle;
        Q_EMIT orientationAngleChanged(angle);
    }
}

QString MirSurfaceItem::name() const
{
    if (m_surface) {
        return m_surface->name();
    } else {
        return QString();
    }
}

bool MirSurfaceItem::live() const
{
    return m_surface && m_surface->live();
}

Mir::ShellChrome MirSurfaceItem::shellChrome() const
{
    return m_surface ? m_surface->shellChrome() : Mir::NormalChrome;
}

// Called from the rendering (scene graph) thread
QSGTextureProvider *MirSurfaceItem::textureProvider() const
{
    QMutexLocker mutexLocker(const_cast<QMutex*>(&m_mutex));
    const_cast<MirSurfaceItem *>(this)->ensureTextureProvider();
    return m_textureProvider;
}

void MirSurfaceItem::ensureTextureProvider()
{
    if (!m_surface) {
        return;
    }

    if (!m_textureProvider) {
        m_textureProvider = new MirTextureProvider(m_surface->texture());

    // Check that the item is indeed using the texture from the MirSurface it currently holds
    // If until now we were drawing a MirSurface "A" and it replaced with a MirSurface "B",
    // we will still hold the texture from "A" until the first time we're asked to draw "B".
    // That's the moment when we finally discard the texture from "A" and get the one from "B".
    //
    // Also note that m_surface->weakTexture() will return null if m_surface->texture() was never
    // called before.
    } else if (!m_textureProvider->texture() || m_textureProvider->texture() != m_surface->weakTexture()) {
        m_textureProvider->setTexture(m_surface->texture());
    }
}

QSGNode *MirSurfaceItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)    // called by render thread
{
    QMutexLocker mutexLocker(&m_mutex);

    if (!m_surface) {
        if (m_textureProvider) {
            m_textureProvider->releaseTexture();
        }
        delete oldNode;
        return 0;
    }

    ensureTextureProvider();

    if (!m_textureProvider->texture() || !m_surface->updateTexture()) {
        delete oldNode;
        return 0;
    }

    if (m_surface->numBuffersReadyForCompositor() > 0) {
        QTimer::singleShot(0, this, &MirSurfaceItem::update);
    }

    m_textureProvider->smooth = smooth();

    QSGDefaultImageNode *node = static_cast<QSGDefaultImageNode*>(oldNode);
    if (!node) {
        node = new QSGDefaultImageNode;
        node->setMipmapFiltering(QSGTexture::None);
        node->setHorizontalWrapMode(QSGTexture::ClampToEdge);
        node->setVerticalWrapMode(QSGTexture::ClampToEdge);
    } else {
        if (!m_lastFrameNumberRendered  || (*m_lastFrameNumberRendered != m_surface->currentFrameNumber())) {
            node->markDirty(QSGNode::DirtyMaterial);
        }
    }
    node->setTexture(m_textureProvider->texture());

    if (m_fillMode == PadOrCrop) {
        const QSize &textureSize = m_textureProvider->texture()->textureSize();

        QRectF targetRect;
        targetRect.setWidth(qMin(width(), static_cast<qreal>(textureSize.width())));
        targetRect.setHeight(qMin(height(), static_cast<qreal>(textureSize.height())));

        qreal u = targetRect.width() / textureSize.width();
        qreal v = targetRect.height() / textureSize.height();
        node->setSubSourceRect(QRectF(0, 0, u, v));

        node->setTargetRect(targetRect);
        node->setInnerTargetRect(targetRect);
    } else {
        // Stretch
        node->setSubSourceRect(QRectF(0, 0, 1, 1));
        node->setTargetRect(QRectF(0, 0, width(), height()));
        node->setInnerTargetRect(QRectF(0, 0, width(), height()));
    }

    node->setFiltering(smooth() ? QSGTexture::Linear : QSGTexture::Nearest);
    node->setAntialiasing(antialiasing());

    node->update();

    if (!m_lastFrameNumberRendered) {
        m_lastFrameNumberRendered = new unsigned int;
    }
    *m_lastFrameNumberRendered = m_surface->currentFrameNumber();

    return node;
}

void MirSurfaceItem::mousePressEvent(QMouseEvent *event)
{
    auto mousePos = event->localPos().toPoint();
    if (m_consumesInput && m_surface && m_surface->live() && m_surface->inputAreaContains(mousePos)) {
        m_surface->mousePressEvent(event);
    } else {
        event->ignore();
    }
}

void MirSurfaceItem::mouseMoveEvent(QMouseEvent *event)
{
    if (m_consumesInput && m_surface && m_surface->live()) {
        m_surface->mouseMoveEvent(event);
    } else {
        event->ignore();
    }
}

void MirSurfaceItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_consumesInput && m_surface && m_surface->live()) {
        m_surface->mouseReleaseEvent(event);
    } else {
        event->ignore();
    }
}

void MirSurfaceItem::wheelEvent(QWheelEvent *event)
{
    if (m_consumesInput && m_surface && m_surface->live()) {
        m_surface->wheelEvent(event);
    } else {
        event->ignore();
    }
}

void MirSurfaceItem::hoverEnterEvent(QHoverEvent *event)
{
    if (m_consumesInput && m_surface && m_surface->live()) {
        m_surface->hoverEnterEvent(event);
    } else {
        event->ignore();
    }
}

void MirSurfaceItem::hoverLeaveEvent(QHoverEvent *event)
{
    if (m_consumesInput && m_surface && m_surface->live()) {
        m_surface->hoverLeaveEvent(event);
    } else {
        event->ignore();
    }
}

void MirSurfaceItem::hoverMoveEvent(QHoverEvent *event)
{
    if (m_consumesInput && m_surface && m_surface->live()) {
        m_surface->hoverMoveEvent(event);
    } else {
        event->ignore();
    }
}

void MirSurfaceItem::keyPressEvent(QKeyEvent *event)
{
    if (m_consumesInput && m_surface && m_surface->live()) {
        m_surface->keyPressEvent(event);
    } else {
        event->ignore();
    }
}

void MirSurfaceItem::keyReleaseEvent(QKeyEvent *event)
{
    if (m_consumesInput && m_surface && m_surface->live()) {
        m_surface->keyReleaseEvent(event);
    } else {
        event->ignore();
    }
}

QString MirSurfaceItem::appId() const
{
    if (m_surface) {
        return m_surface->appId();
    } else {
        return QStringLiteral("-");
    }
}

void MirSurfaceItem::endCurrentTouchSequence(ulong timestamp)
{
    Q_ASSERT(m_lastTouchEvent);
    Q_ASSERT(m_lastTouchEvent->type != QEvent::TouchEnd);
    Q_ASSERT(m_lastTouchEvent->touchPoints.count() > 0);

    TouchEvent touchEvent = *m_lastTouchEvent;
    touchEvent.timestamp = timestamp;

    // Remove all already released touch points
    int i = 0;
    while (i < touchEvent.touchPoints.count()) {
        if (touchEvent.touchPoints[i].state() == Qt::TouchPointReleased) {
            touchEvent.touchPoints.removeAt(i);
        } else {
            ++i;
        }
    }

    // And release the others one by one as Mir expects one press/release per event
    while (touchEvent.touchPoints.count() > 0) {
        touchEvent.touchPoints[0].setState(Qt::TouchPointReleased);

        touchEvent.updateTouchPointStatesAndType();

        m_surface->touchEvent(touchEvent.modifiers, touchEvent.touchPoints,
                               touchEvent.touchPointStates, touchEvent.timestamp);

        *m_lastTouchEvent = touchEvent;

        touchEvent.touchPoints.removeAt(0);
    }
}

void MirSurfaceItem::validateAndDeliverTouchEvent(int eventType,
            ulong timestamp,
            Qt::KeyboardModifiers mods,
            const QList<QTouchEvent::TouchPoint> &touchPoints,
            Qt::TouchPointStates touchPointStates)
{
    if (eventType == QEvent::TouchBegin && m_lastTouchEvent && m_lastTouchEvent->type != QEvent::TouchEnd) {
        qCWarning(QTMIR_SURFACES) << qPrintable(QStringLiteral("MirSurfaceItem(%1) - Got a QEvent::TouchBegin while "
            "there's still an active/unfinished touch sequence.").arg(appId()));
        // Qt forgot to end the last touch sequence. Let's do it ourselves.
        endCurrentTouchSequence(timestamp);
    }

    m_surface->touchEvent(mods, touchPoints, touchPointStates, timestamp);

    if (!m_lastTouchEvent) {
        m_lastTouchEvent = new TouchEvent;
    }
    m_lastTouchEvent->type = eventType;
    m_lastTouchEvent->timestamp = timestamp;
    m_lastTouchEvent->touchPoints = touchPoints;
    m_lastTouchEvent->touchPointStates = touchPointStates;

    tracepoint(qtmir, touchEventConsume_end, uncompressTimestamp<ulong>(timestamp).count());
}

void MirSurfaceItem::touchEvent(QTouchEvent *event)
{
    tracepoint(qtmir, touchEventConsume_start, uncompressTimestamp<ulong>(event->timestamp()).count());

    bool accepted = processTouchEvent(event->type(),
            event->timestamp(),
            event->modifiers(),
            event->touchPoints(),
            event->touchPointStates());
    event->setAccepted(accepted);
}

bool MirSurfaceItem::processTouchEvent(
        int eventType,
        ulong timestamp,
        Qt::KeyboardModifiers mods,
        const QList<QTouchEvent::TouchPoint> &touchPoints,
        Qt::TouchPointStates touchPointStates)
{

    if (!m_consumesInput || !m_surface || !m_surface->live()) {
        return false;
    }

    if (eventType == QEvent::TouchBegin && !hasTouchInsideInputRegion(touchPoints)) {
        return false;
    }

    validateAndDeliverTouchEvent(eventType, timestamp, mods, touchPoints, touchPointStates);

    return true;
}

bool MirSurfaceItem::hasTouchInsideInputRegion(const QList<QTouchEvent::TouchPoint> &touchPoints)
{
    for (int i = 0; i < touchPoints.count(); ++i) {
        QPoint pos = touchPoints.at(i).pos().toPoint();
        if (m_surface->inputAreaContains(pos)) {
            return true;
        }
    }
    return false;
}

Mir::State MirSurfaceItem::surfaceState() const
{
    if (m_surface) {
        return m_surface->state();
    } else {
        return Mir::UnknownState;
    }
}

void MirSurfaceItem::setSurfaceState(Mir::State state)
{
    if (m_surface) {
        m_surface->setState(state);
    }
}

void MirSurfaceItem::scheduleMirSurfaceSizeUpdate()
{
    if (!m_updateMirSurfaceSizeTimer.isActive()) {
        m_updateMirSurfaceSizeTimer.start();
    }
}

void MirSurfaceItem::updateMirSurfaceSize()
{
    if (!m_surface || !m_surface->live() || (m_surfaceWidth <= 0 && m_surfaceHeight <= 0)) {
        return;
    }

    // If one dimension is not set, fallback to the current value
    int width = m_surfaceWidth > 0 ? m_surfaceWidth : m_surface->size().width();
    int height = m_surfaceHeight > 0 ? m_surfaceHeight : m_surface->size().height();

    m_surface->resize(width, height);
}

void MirSurfaceItem::updateMirSurfaceVisibility()
{
    if (!m_surface || !m_surface->live()) {
        return;
    }

    m_surface->setViewVisibility((qintptr)this, isVisible());
}

void MirSurfaceItem::updateMirSurfaceActiveFocus()
{
    if (m_surface && m_surface->live()) {
        m_surface->setViewActiveFocus(qintptr(this), m_consumesInput && hasActiveFocus());
    }
}

void MirSurfaceItem::invalidateSceneGraph()
{
    delete m_textureProvider;
    m_textureProvider = nullptr;
}

void MirSurfaceItem::TouchEvent::updateTouchPointStatesAndType()
{
    touchPointStates = 0;
    for (int i = 0; i < touchPoints.count(); ++i) {
        touchPointStates |= touchPoints.at(i).state();
    }

    if (touchPointStates == Qt::TouchPointReleased) {
        type = QEvent::TouchEnd;
    } else if (touchPointStates == Qt::TouchPointPressed) {
        type = QEvent::TouchBegin;
    } else {
        type = QEvent::TouchUpdate;
    }
}

bool MirSurfaceItem::consumesInput() const
{
    return m_consumesInput;
}

void MirSurfaceItem::setConsumesInput(bool value)
{
    if (m_consumesInput == value) {
        return;
    }

    m_consumesInput = value;
    if (m_consumesInput) {
        setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton |
            Qt::ExtraButton1 | Qt::ExtraButton2 | Qt::ExtraButton3 | Qt::ExtraButton4 |
            Qt::ExtraButton5 | Qt::ExtraButton6 | Qt::ExtraButton7 | Qt::ExtraButton8 |
            Qt::ExtraButton9 | Qt::ExtraButton10 | Qt::ExtraButton11 |
            Qt::ExtraButton12 | Qt::ExtraButton13);
        setAcceptHoverEvents(true);
    } else {
        setAcceptedMouseButtons(Qt::NoButton);
        setAcceptHoverEvents(false);
    }

    updateMirSurfaceActiveFocus();
    Q_EMIT consumesInputChanged(value);
}

unity::shell::application::MirSurfaceInterface* MirSurfaceItem::surface() const
{
    return m_surface;
}

void MirSurfaceItem::setSurface(unity::shell::application::MirSurfaceInterface *unitySurface)
{
    QMutexLocker mutexLocker(&m_mutex);

    auto surface = static_cast<qtmir::MirSurfaceInterface*>(unitySurface);
    qCDebug(QTMIR_SURFACES).nospace() << "MirSurfaceItem::setSurface surface=" << surface;

    if (surface == m_surface) {
        return;
    }

    if (m_surface) {
        disconnect(m_surface, nullptr, this, nullptr);
        m_surface->unregisterView((qintptr)this);
        unsetCursor();
    }

    m_surface = surface;

    if (m_surface) {
        m_surface->registerView((qintptr)this);

        // When a new mir frame gets posted we notify the QML engine that this item needs redrawing,
        // schedules call to updatePaintNode() from the rendering thread
        connect(m_surface, &MirSurfaceInterface::framesPosted, this, &QQuickItem::update);

        connect(m_surface, &MirSurfaceInterface::stateChanged, this, &MirSurfaceItem::surfaceStateChanged);
        connect(m_surface, &MirSurfaceInterface::liveChanged, this, &MirSurfaceItem::liveChanged);
        connect(m_surface, &MirSurfaceInterface::sizeChanged, this, &MirSurfaceItem::onActualSurfaceSizeChanged);
        connect(m_surface, &MirSurfaceInterface::cursorChanged, this, &MirSurfaceItem::setCursor);
        connect(m_surface, &MirSurfaceInterface::shellChromeChanged, this, &MirSurfaceItem::shellChromeChanged);

        Q_EMIT typeChanged(m_surface->type());
        Q_EMIT liveChanged(true);
        Q_EMIT surfaceStateChanged(m_surface->state());

        updateMirSurfaceSize();
        setImplicitSize(m_surface->size().width(), m_surface->size().height());
        updateMirSurfaceVisibility();
        if (window()) {
            updateScreen(window()->screen());
        }

        // Qt::ArrowCursor is the default when no cursor has been explicitly set, so no point forwarding it.
        if (m_surface->cursor().shape() != Qt::ArrowCursor) {
            setCursor(m_surface->cursor());
        }

        if (m_orientationAngle) {
            m_surface->setOrientationAngle(*m_orientationAngle);
            connect(m_surface, &MirSurfaceInterface::orientationAngleChanged, this, &MirSurfaceItem::orientationAngleChanged);
            delete m_orientationAngle;
            m_orientationAngle = nullptr;
        } else {
            connect(m_surface, &MirSurfaceInterface::orientationAngleChanged, this, &MirSurfaceItem::orientationAngleChanged);
            Q_EMIT orientationAngleChanged(m_surface->orientationAngle());
        }

        updateMirSurfaceActiveFocus();
    }

    update();

    Q_EMIT surfaceChanged(m_surface);
}

void MirSurfaceItem::onCompositorSwappedBuffers()
{
    if (Q_LIKELY(m_surface)) {
        m_surface->onCompositorSwappedBuffers();
    }
}

void MirSurfaceItem::onWindowChanged(QQuickWindow *window)
{
    if (m_window) {
        disconnect(m_window, nullptr, this, nullptr);
    }
    m_window = window;
    if (m_window) {
        connect(m_window, &QQuickWindow::frameSwapped, this, &MirSurfaceItem::onCompositorSwappedBuffers,
                Qt::DirectConnection);

        updateScreen(m_window->screen());
        connect(m_window, &QQuickWindow::screenChanged, this, &MirSurfaceItem::updateScreen);
    }
}

void MirSurfaceItem::updateScreen(QScreen *screen)
{
    if (screen && m_surface) {
        m_surface->setScreen(screen);
    }
}

void MirSurfaceItem::releaseResources()
{
    if (m_textureProvider) {
        Q_ASSERT(window());

        MirSurfaceItemReleaseResourcesJob *job = new MirSurfaceItemReleaseResourcesJob;
        job->textureProvider = m_textureProvider;
        m_textureProvider = nullptr;
        window()->scheduleRenderJob(job, QQuickWindow::AfterSynchronizingStage);
    }
}

int MirSurfaceItem::surfaceWidth() const
{
    return m_surfaceWidth;
}

void MirSurfaceItem::setSurfaceWidth(int value)
{
    if (value != m_surfaceWidth) {
        m_surfaceWidth = value;
        scheduleMirSurfaceSizeUpdate();
        Q_EMIT surfaceWidthChanged(value);
    }
}

void MirSurfaceItem::onActualSurfaceSizeChanged(QSize size)
{
    setImplicitSize(size.width(), size.height());
}

int MirSurfaceItem::surfaceHeight() const
{
    return m_surfaceHeight;
}

void MirSurfaceItem::setSurfaceHeight(int value)
{
    if (value != m_surfaceHeight) {
        m_surfaceHeight = value;
        scheduleMirSurfaceSizeUpdate();
        Q_EMIT surfaceHeightChanged(value);
    }
}

MirSurfaceItem::FillMode MirSurfaceItem::fillMode() const
{
    return m_fillMode;
}

void MirSurfaceItem::setFillMode(FillMode value)
{
    if (m_fillMode != value) {
        m_fillMode = value;
        Q_EMIT fillModeChanged(m_fillMode);
    }
}

} // namespace qtmir

#include "mirsurfaceitem.moc"
