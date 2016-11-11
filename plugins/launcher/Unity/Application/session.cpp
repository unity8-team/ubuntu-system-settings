/*
 * Copyright (C) 2014-2016 Canonical, Ltd.
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
#include "debughelpers.h"
#include "session.h"
#include "mirsurfaceinterface.h"
#include "mirsurfacemanager.h"
#include "mirsurfaceitem.h"

// mirserver
#include "logging.h"

// mir
#include <mir/scene/session.h>
#include <mir/scene/prompt_session.h>
#include <mir/scene/prompt_session_manager.h>

// Qt
#include <QPainter>
#include <QQmlEngine>

namespace ms = mir::scene;

using unity::shell::application::ApplicationInfoInterface;

#define DEBUG_MSG qCDebug(QTMIR_SURFACES).nospace() << "Session[" << (void*)this << ",name=" << name() << "]::" << __func__

namespace qtmir
{

namespace {

const char *sessionStateToString(SessionInterface::State state)
{
    switch (state) {
    case SessionInterface::Starting:
        return "starting";
    case SessionInterface::Running:
        return "running";
    case SessionInterface::Suspending:
        return "suspending";
    case SessionInterface::Suspended:
        return "suspended";
    case SessionInterface::Stopped:
        return "stopped";
    default:
        return "???";
    }
}

}

Session::Session(const std::shared_ptr<ms::Session>& session,
                 const std::shared_ptr<ms::PromptSessionManager>& promptSessionManager,
                 QObject *parent)
    : SessionInterface(parent)
    , m_session(session)
    , m_application(nullptr)
    , m_children(new SessionModel(this))
    , m_fullscreen(false)
    , m_state(State::Starting)
    , m_live(true)
    , m_promptSessionManager(promptSessionManager)
{
    DEBUG_MSG << "()";

    setSuspendTimer(new Timer);

    connect(&m_surfaceList, &MirSurfaceListModel::emptyChanged, this, &Session::deleteIfZombieAndEmpty);
}

Session::~Session()
{
    DEBUG_MSG << "()";
    stopPromptSessions();

    const QList<SessionInterface*> children(m_children->list());
    for (SessionInterface* child : children) {
        delete child;
    }
    if (m_application) {
        m_application->setSession(nullptr);
    }

    delete m_children; m_children = nullptr;

    delete m_suspendTimer;

    Q_EMIT destroyed(this); // Early warning, while Session methods can still be accessed.
}

void Session::doSuspend()
{
    Q_ASSERT(m_state == Session::Suspending);

    if (m_surfaceList.count() == 0) {
        DEBUG_MSG << " no surface to call stopFrameDropper() on!";
    } else {
        for (int i = 0; i < m_surfaceList.count(); ++i) {
            auto surface = static_cast<MirSurfaceInterface*>(m_surfaceList.get(i));
            surface->stopFrameDropper();
        }
    }
    setState(Suspended);
}

QString Session::name() const
{
    return QString::fromStdString(m_session->name());
}

std::shared_ptr<ms::Session> Session::session() const
{
    return m_session;
}

ApplicationInfoInterface* Session::application() const
{
    return m_application;
}

MirSurfaceListModel* Session::surfaceList()
{
    return &m_surfaceList;
}

MirSurfaceListModel* Session::promptSurfaceList()
{
    return &m_promptSurfaceList;
}

Session::State Session::state() const
{
    return m_state;
}

void Session::setState(State state)
{
    if (m_state == state) {
        return;
    }

    DEBUG_MSG << "(state=" << sessionStateToString(state) << ")";

    if (m_state == Suspending) {
        m_suspendTimer->stop();
    }

    m_state = state;

    switch (m_state) {
        case Starting:
        case Running:
            break;
        case Suspending:
            m_suspendTimer->start();
            break;
        case Suspended:
        case Stopped:
            break;
    }

    Q_EMIT stateChanged(m_state);
}

bool Session::fullscreen() const
{
    return m_fullscreen;
}

bool Session::live() const
{
    return m_live;
}

void Session::setApplication(ApplicationInfoInterface* application)
{
    if (m_application == application)
        return;

    m_application = static_cast<Application*>(application);
    Q_EMIT applicationChanged(application);
}

void Session::registerSurface(MirSurfaceInterface *newSurface)
{
    DEBUG_MSG << "(surface=" << newSurface << ")";

    // Only notify QML of surface creation once it has drawn its first frame.
    if (newSurface->isFirstFrameDrawn()) {
        prependSurface(newSurface);
    } else {
        connect(newSurface, &MirSurfaceInterface::firstFrameDrawn, this, [this, newSurface]()
            {
                newSurface->disconnect(this);
                this->prependSurface(newSurface);
            });
    }
}

void Session::prependSurface(MirSurfaceInterface *newSurface)
{
    DEBUG_MSG << "(surface=" << newSurface << ")";

    connect(newSurface, &MirSurfaceInterface::stateChanged,
        this, &Session::updateFullscreenProperty);

    connect(newSurface, &MirSurfaceInterface::closeRequested, this, [this, newSurface]()
        {
            m_closingSurfaces.append(newSurface);
            if (m_closingSurfaces.count() == 1) {
                Q_EMIT hasClosingSurfacesChanged();
            }

            m_surfaceList.removeSurface(newSurface);
        });
    connect(newSurface, &QObject::destroyed, this, [this, newSurface]()
        {
            this->removeSurface(newSurface);
        });
    connect(newSurface, &MirSurfaceInterface::focusRequested, this, &SessionInterface::focusRequested);

    m_surfaceList.prependSurface(newSurface);
    m_hadSurface = true;

    if (m_state == Starting) {
        setState(Running);
    }

    updateFullscreenProperty();
}

void Session::removeSurface(MirSurfaceInterface* surface)
{
    DEBUG_MSG << "(surface=" << surface << ")";

    surface->disconnect(this);

    if (m_surfaceList.contains(surface)) {
        m_surfaceList.removeSurface(surface);
    }

    if (m_closingSurfaces.contains(surface)) {
        m_closingSurfaces.removeAll(surface);
        if (m_closingSurfaces.isEmpty()) {
            Q_EMIT hasClosingSurfacesChanged();
        }
    }

    updateFullscreenProperty();
}

void Session::updateFullscreenProperty()
{
    if (m_surfaceList.count() > 0) {
        // TODO: Figure out something better
        setFullscreen(m_surfaceList.get(0)->state() == Mir::FullscreenState);
    } else {
        // Keep the current value of the fullscreen property until we get a new
        // surface
    }
}

void Session::setFullscreen(bool fullscreen)
{
    if (m_fullscreen != fullscreen) {
        DEBUG_MSG << "(" << fullscreen << ")";
        m_fullscreen = fullscreen;
        Q_EMIT fullscreenChanged(m_fullscreen);
    }
}

void Session::suspend()
{
    DEBUG_MSG << " state=" << sessionStateToString(m_state);
    if (m_state == Running) {
        session()->set_lifecycle_state(mir_lifecycle_state_will_suspend);
        m_suspendTimer->start();

        foreachPromptSession([this](const std::shared_ptr<ms::PromptSession>& promptSession) {
            m_promptSessionManager->suspend_prompt_session(promptSession);
        });

        foreachChildSession([](SessionInterface* session) {
            session->suspend();
        });

        setState(Suspending);
    }
}

void Session::resume()
{
    DEBUG_MSG << " state=" << sessionStateToString(m_state);

    if (m_state == Suspending || m_state == Suspended) {
        doResume();
    }
}

void Session::doResume()
{
    if (m_state == Suspended) {
        for (int i = 0; i < m_surfaceList.count(); ++i) {
            auto surface = static_cast<MirSurfaceInterface*>(m_surfaceList.get(i));
            surface->startFrameDropper();
        }
    }

    session()->set_lifecycle_state(mir_lifecycle_state_resumed);

    foreachPromptSession([this](const std::shared_ptr<ms::PromptSession>& promptSession) {
        m_promptSessionManager->resume_prompt_session(promptSession);
    });

    foreachChildSession([](SessionInterface* session) {
        session->resume();
    });

    setState(Running);
}

void Session::close()
{
    DEBUG_MSG << "()";

    if (m_state == Stopped) return;

    // surface::close ends up removing the surface from the list.
    // Reverse iteration order to preserve the index.
    for (int i = m_surfaceList.count()-1; i >= 0; --i) {
        auto surface = static_cast<MirSurfaceInterface*>(m_surfaceList.get(i));
        surface->close();
    }
}

void Session::stop()
{
    DEBUG_MSG << "()";

    if (m_state != Stopped) {

        stopPromptSessions();

        {
            for (int i = 0; i < m_surfaceList.count(); ++i) {
                auto surface = static_cast<MirSurfaceInterface*>(m_surfaceList.get(i));
                surface->stopFrameDropper();
            }
        }

        foreachChildSession([](SessionInterface* session) {
            session->stop();
        });

        setState(Stopped);
    }
}

void Session::setLive(const bool live)
{
    if (m_live != live) {
        DEBUG_MSG << "(" << live << ")";

        m_live = live;
        Q_EMIT liveChanged(m_live);
        if (!live) {
            setState(Stopped);

            for (int i = 0; i < m_surfaceList.count(); ++i) {
                auto surface = static_cast<MirSurfaceInterface*>(m_surfaceList.get(i));
                surface->setLive(false);
            }

            deleteIfZombieAndEmpty();
        }
    }
}

void Session::addChildSession(SessionInterface* session)
{
    insertChildSession(m_children->rowCount(), session);
}

void Session::insertChildSession(uint index, SessionInterface* session)
{
    DEBUG_MSG << "(index=" << index << ", Session[" << (void*)session << ",name=" << session->name() << "])";
    Q_ASSERT(!m_children->contains(session));

    m_children->insert(index, session);

    // Flatten the list of prompt surfaces
    m_promptSurfaceList.addSurfaceList(session->surfaceList());
    m_promptSurfaceList.addSurfaceList(session->promptSurfaceList());

    connect(session, &QObject::destroyed, this, [this, session]() { this->removeChildSession(session); });

    switch (m_state) {
        case Starting:
        case Running:
            session->resume();
            break;
        case Suspending:
        case Suspended:
            session->suspend();
            break;
        case Stopped:
            session->stop();
            break;
    }
}

void Session::removeChildSession(SessionInterface* session)
{
    DEBUG_MSG << "(Session[" << (void*)session << ",name=" << session->name() << "])";

    disconnect(session, 0, this, 0);

    if (m_children->contains(session)) {
        m_children->remove(session);
        m_promptSurfaceList.removeSurfaceList(session->surfaceList());
        m_promptSurfaceList.removeSurfaceList(session->promptSurfaceList());
    }

    deleteIfZombieAndEmpty();
}

void Session::foreachChildSession(const std::function<void(SessionInterface* session)>& f) const
{
    const QList<SessionInterface*> children(m_children->list());
    for (SessionInterface* child : children) {
        f(child);
    }
}

SessionModel* Session::childSessions() const
{
    return m_children;
}

void Session::appendPromptSession(const std::shared_ptr<ms::PromptSession>& promptSession)
{
    DEBUG_MSG << "(promptSession=" << (promptSession ? promptSession.get() : nullptr) << ")";

    m_promptSessions.append(promptSession);
}

void Session::removePromptSession(const std::shared_ptr<ms::PromptSession>& promptSession)
{
    DEBUG_MSG << "(promptSession=" << (promptSession ? promptSession.get() : nullptr) << ")";

    m_promptSessions.removeAll(promptSession);
}

void Session::stopPromptSessions()
{
    const QList<SessionInterface*> children(m_children->list());
    for (SessionInterface* child : children) {
        static_cast<Session*>(child)->stopPromptSessions();
    }

    QVector<std::shared_ptr<ms::PromptSession>> copy(m_promptSessions);
    QVectorIterator<std::shared_ptr<ms::PromptSession>> it(copy);
    for ( it.toBack(); it.hasPrevious(); ) {
        std::shared_ptr<ms::PromptSession> promptSession = it.previous();
        DEBUG_MSG << " - promptSession=" << promptSession.get();

        m_promptSessionManager->stop_prompt_session(promptSession);
    }
}

std::shared_ptr<ms::PromptSession> Session::activePromptSession() const
{
    if (m_promptSessions.count() > 0)
        return m_promptSessions.back();
    return nullptr;
}

void Session::foreachPromptSession(const std::function<void(const std::shared_ptr<ms::PromptSession>&)>& f) const
{
    Q_FOREACH (std::shared_ptr<ms::PromptSession> promptSession, m_promptSessions) {
        f(promptSession);
    }
}

void Session::deleteIfZombieAndEmpty()
{
    if (!m_live && m_children->rowCount() == 0 && m_surfaceList.isEmpty()) {
        DEBUG_MSG << " - deleteLater()";
        deleteLater();
    }
}

bool Session::hasClosingSurfaces() const
{
    return m_closingSurfaces.count() > 0;
}

bool Session::hadSurface() const
{
    return m_hadSurface;
}

bool Session::activeFocus() const
{
    for (int i = 0; i < m_surfaceList.count(); ++i) {
        auto surface = static_cast<const MirSurfaceInterface*>(m_surfaceList.get(i));
        if (surface->activeFocus()) {
            return true;
        }
    }

    return false;
}

pid_t Session::pid() const
{
    return m_session->process_id();
}

void Session::setSuspendTimer(AbstractTimer *timer)
{
    bool timerWasRunning = false;

    if (m_suspendTimer) {
        timerWasRunning = m_suspendTimer->isRunning();
        delete m_suspendTimer;
    }

    m_suspendTimer = timer;
    m_suspendTimer->setInterval(1500);
    m_suspendTimer->setSingleShot(true);
    connect(m_suspendTimer, &AbstractTimer::timeout, this, &Session::doSuspend);

    if (timerWasRunning) {
        m_suspendTimer->start();
    }
}

} // namespace qtmir
