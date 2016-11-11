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
#include "applicationinfo.h"
#include "application_manager.h"
#include "mirsurfaceinterface.h"
#include "session.h"
#include "sharedwakelock.h"
#include "timer.h"

// common
#include <debughelpers.h>

// QPA mirserver
#include "logging.h"

// Unity API
#include <unity/shell/application/MirSurfaceInterface.h>

namespace unityapp = unity::shell::application;

#define DEBUG_MSG qCDebug(QTMIR_APPLICATIONS).nospace() << "Application[" << appId() <<"]::" << __func__

namespace qtmir
{

Application::Application(const QSharedPointer<SharedWakelock>& sharedWakelock,
                         const QSharedPointer<ApplicationInfo>& appInfo,
                         const QStringList &arguments,
                         ApplicationManager *parent)
    : ApplicationInfoInterface(appInfo->appId(), parent)
    , m_sharedWakelock(sharedWakelock)
    , m_appInfo(appInfo)
    , m_pid(0)
    , m_supportedStages(Application::MainStage|Application::SideStage)
    , m_state(InternalState::Starting)
    , m_arguments(arguments)
    , m_session(nullptr)
    , m_requestedState(RequestedRunning)
    , m_processState(ProcessUnknown)
    , m_stopTimer(nullptr)
    , m_exemptFromLifecycle(false)
    , m_proxySurfaceList(new ProxySurfaceListModel(this))
    , m_proxyPromptSurfaceList(new ProxySurfaceListModel(this))
{
    DEBUG_MSG << "()";

    // Because m_state is InternalState::Starting
    acquireWakelock();

    m_supportedOrientations = m_appInfo->supportedOrientations();

    m_rotatesWindowContents = m_appInfo->rotatesWindowContents();

    setStopTimer(new Timer);

    connect(m_proxySurfaceList, &unityapp::MirSurfaceListInterface::countChanged, this, &unityapp::ApplicationInfoInterface::surfaceCountChanged);
}

Application::~Application()
{
    DEBUG_MSG << "()";

    // (ricmm) -- To be on the safe side, better wipe the application QML compile cache if it crashes on startup
    if (m_processState == Application::ProcessUnknown) {
        wipeQMLCache();
    }

    switch (m_state) {
    case InternalState::Starting:
    case InternalState::Running:
    case InternalState::RunningInBackground:
    case InternalState::SuspendingWaitSession:
    case InternalState::SuspendingWaitProcess:
        wipeQMLCache();
        break;
    case InternalState::Closing:
    case InternalState::Suspended:
    case InternalState::StoppedResumable:
        break;
    case InternalState::Stopped:
        if (m_processState == Application::ProcessFailed) { // process crashed
            wipeQMLCache();
        }
        break;
    }

    if (m_session) {
        m_session->setApplication(nullptr);
        delete m_session;
    }
    delete m_stopTimer;
}


void Application::wipeQMLCache()
{
    QString path(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QStringLiteral("/QML/Apps/"));
    QDir dir(path);
    QStringList apps = dir.entryList();
    for (int i = 0; i < apps.size(); i++) {
        if (apps.at(i).contains(appId())) {
            qCDebug(QTMIR_APPLICATIONS) << "Application appId=" << apps.at(i) << " Wiping QML Cache";
            dir.cd(apps.at(i));
            dir.removeRecursively();
            break;
        }
    }
}

bool Application::isValid() const
{
    return !appId().isEmpty();
}

QString Application::appId() const
{
    return m_appInfo->appId();
}

QString Application::name() const
{
    return m_appInfo->name();
}

QString Application::comment() const
{
    return m_appInfo->comment();
}

QUrl Application::icon() const
{
    return m_appInfo->icon();
}

QString Application::splashTitle() const
{
    return m_appInfo->splashTitle();
}

QUrl Application::splashImage() const
{
    return m_appInfo->splashImage();
}

QColor Application::colorFromString(const QString &colorString, const char *colorName) const
{
    // NB: If a colour which is not fully opaque is specified in the desktop file, it will
    //     be ignored and the default colour will be used instead.
    QColor color;

    if (colorString.isEmpty()) {
        color.setRgba(qRgba(0, 0, 0, 0));
    } else {
        color.setNamedColor(colorString);

        if (color.isValid()) {
            // Force a fully opaque color.
            color.setAlpha(255);
        } else {
            color.setRgba(qRgba(0, 0, 0, 0));
            qCWarning(QTMIR_APPLICATIONS) << QStringLiteral("Invalid %1: \"%2\"")
                .arg(colorName, colorString);
        }
    }

    return color;
}

const char* Application::internalStateToStr(InternalState state)
{
    switch (state) {
    case InternalState::Starting:
        return "Starting";
    case InternalState::Running:
        return "Running";
    case InternalState::RunningInBackground:
        return "RunningInBackground";
    case InternalState::SuspendingWaitSession:
        return "SuspendingWaitSession";
    case InternalState::SuspendingWaitProcess:
        return "SuspendingWaitProcess";
    case InternalState::Suspended:
        return "Suspended";
    case InternalState::Closing:
        return "Closing";
    case InternalState::StoppedResumable:
        return "StoppedResumable";
    case InternalState::Stopped:
        return "Stopped";
    default:
        return "???";
    }
}

bool Application::splashShowHeader() const
{
    return m_appInfo->splashShowHeader();
}

QColor Application::splashColor() const
{
    QString colorStr = m_appInfo->splashColor();
    return colorFromString(colorStr, "splashColor");
}

QColor Application::splashColorHeader() const
{
    QString colorStr = m_appInfo->splashColorHeader();
    return colorFromString(colorStr, "splashColorHeader");
}

QColor Application::splashColorFooter() const
{
    QString colorStr = m_appInfo->splashColorFooter();
    return colorFromString(colorStr, "splashColorFooter");
}

Application::Stages Application::supportedStages() const
{
    return m_supportedStages;
}

Application::State Application::state() const
{
    // The public state is a simplified version of the internal one as our consumers
    // don't have to know or care about all the nasty details.
    switch (m_state) {
    case InternalState::Starting:
        return Starting;
    case InternalState::Running:
    case InternalState::RunningInBackground:
    case InternalState::Closing:
        return Running;
    case InternalState::SuspendingWaitSession:
    case InternalState::SuspendingWaitProcess:
    case InternalState::Suspended:
        return Suspended;
    case InternalState::StoppedResumable:
    case InternalState::Stopped:
    default:
        return Stopped;
    }
}

Application::RequestedState Application::requestedState() const
{
    return m_requestedState;
}

void Application::setRequestedState(RequestedState value)
{
    if (m_requestedState == value) {
        // nothing to do
        return;
    }

    DEBUG_MSG << "(requestedState=" << applicationStateToStr(value) << ")";

    m_requestedState = value;
    Q_EMIT requestedStateChanged(m_requestedState);

    updateState();
}

void Application::updateState()
{
    if ((!m_session && m_state != InternalState::Starting && m_state != InternalState::StoppedResumable)
        ||
        (m_session && m_session->surfaceList()->isEmpty() && m_session->hasClosingSurfaces())) {
        // As we might not be able to go to Closing state right now (eg, SuspendingWaitProcess),
        // store the intent in a separate variable.
        m_closing = true;
    }

    bool lostAllSurfaces = m_session && m_session->surfaceList()->isEmpty() && m_session->hadSurface()
            && !m_session->hasClosingSurfaces();

    if (m_closing || (lostAllSurfaces && m_state != InternalState::StoppedResumable)) {
        applyClosing();
    } else if (m_requestedState == RequestedRunning || (m_session && m_session->hasClosingSurfaces())) {
        applyRequestedRunning();
    } else {
        applyRequestedSuspended();
    }
}

void Application::applyClosing()
{
    switch (m_state) {
    case InternalState::Starting:
        // can't be
        Q_ASSERT(false);
        break;
    case InternalState::Running:
    case InternalState::RunningInBackground:
        if (!m_stopTimer->isRunning()) {
            m_stopTimer->start();
        }
        if (m_closing) {
            setInternalState(InternalState::Closing);
        }
        break;
    case InternalState::SuspendingWaitSession:
    case InternalState::Suspended:
        resume();
        break;
    case InternalState::SuspendingWaitProcess:
        // should leave the app alone until it reaches Suspended state
        break;
    case InternalState::Closing:
        // leave it alone
        Q_ASSERT(m_closing);
        Q_ASSERT(m_stopTimer->isRunning());
        break;
    case InternalState::StoppedResumable:
        setInternalState(InternalState::Stopped);
        break;
    case InternalState::Stopped:
        break;
    }
}

void Application::applyRequestedRunning()
{
    // We might be coming back from having lost all surfaces
    if (m_stopTimer->isRunning()) {
        m_stopTimer->stop();
    }

    switch (m_state) {
    case InternalState::Starting:
        // should leave the app alone until it reaches Running state
        break;
    case InternalState::Running:
        // already where it's wanted to be
        break;
    case InternalState::RunningInBackground:
    case InternalState::SuspendingWaitSession:
    case InternalState::Suspended:
        resume();
        break;
    case InternalState::SuspendingWaitProcess:
        // should leave the app alone until it reaches Suspended state
        break;
    case InternalState::Closing:
        // can't be
        Q_ASSERT(false);
        break;
    case InternalState::StoppedResumable:
        respawn();
        break;
    case InternalState::Stopped:
        // dead end.
        break;
    }
}

void Application::applyRequestedSuspended()
{
    // We might be coming back from having lost all surfaces
    if (m_stopTimer->isRunning()) {
        m_stopTimer->stop();
    }

    switch (m_state) {
    case InternalState::Starting:
        // should leave the app alone until it reaches Running state
        break;
    case InternalState::Running:
        if (m_processState == ProcessRunning) {
            suspend();
        } else {
            // we can't suspend it since we have no information on the app process
            Q_ASSERT(m_processState == ProcessUnknown);
        }
        break;
    case InternalState::RunningInBackground:
    case InternalState::SuspendingWaitSession:
    case InternalState::SuspendingWaitProcess:
    case InternalState::Suspended:
        // it's already going where we it's wanted
        break;
    case InternalState::Closing:
        // can't be
        Q_ASSERT(false);
        break;
    case InternalState::StoppedResumable:
    case InternalState::Stopped:
        // the app doesn't have a process in the first place, so there's nothing to suspend
        break;
    }
}

bool Application::focused() const
{
    bool someSurfaceHasFocus = false; // to be proven wrong
    for (int i = 0; i < m_proxySurfaceList->rowCount() && !someSurfaceHasFocus; ++i) {
        someSurfaceHasFocus |= m_proxySurfaceList->get(i)->focused();
    }
    return someSurfaceHasFocus;
}

bool Application::fullscreen() const
{
    return m_session ? m_session->fullscreen() : false;
}

pid_t Application::pid() const
{
    return m_pid;
}

void Application::close()
{
    DEBUG_MSG << "()";

    switch (m_state) {
    case InternalState::Starting:
        stop();
        // Don't wait for a confirmation.
        setInternalState(InternalState::Stopped);
        break;
    case InternalState::Running:
    case InternalState::RunningInBackground:
    case InternalState::SuspendingWaitSession:
    case InternalState::SuspendingWaitProcess:
    case InternalState::Suspended:
        m_session->close();
        break;
    case InternalState::Closing:
        // already on the way
        break;
    case InternalState::StoppedResumable:
        // session stopped while suspended. Stop it for good now.
        setInternalState(InternalState::Stopped);
        break;
    case InternalState::Stopped:
        // too late
        break;
    }
}

void Application::setPid(pid_t pid)
{
    m_pid = pid;
}

void Application::setArguments(const QStringList &arguments)
{
    m_arguments = arguments;
}

void Application::setSession(SessionInterface *newSession)
{
    DEBUG_MSG << "(session=" << newSession << ")";

    if (newSession == m_session)
        return;

    if (m_session) {
        m_proxySurfaceList->setSourceList(nullptr);
        m_proxyPromptSurfaceList->setSourceList(nullptr);
        m_session->disconnect(this);
        m_session->surfaceList()->disconnect(this);
        m_session->setApplication(nullptr);
        m_session->setParent(nullptr);
    }

    bool oldFullscreen = fullscreen();
    m_session = newSession;

    if (m_session) {
        m_session->setParent(this);
        m_session->setApplication(this);

        switch (m_state) {
        case InternalState::Starting:
        case InternalState::Running:
        case InternalState::RunningInBackground:
        case InternalState::Closing:
            m_session->resume();
            break;
        case InternalState::SuspendingWaitSession:
        case InternalState::SuspendingWaitProcess:
        case InternalState::Suspended:
            m_session->suspend();
            break;
        case InternalState::Stopped:
        default:
            m_session->stop();
            break;
        }

        connect(m_session, &SessionInterface::stateChanged, this, &Application::onSessionStateChanged);
        connect(m_session, &SessionInterface::fullscreenChanged, this, &Application::fullscreenChanged);
        connect(m_session, &SessionInterface::hasClosingSurfacesChanged, this, &Application::updateState);
        connect(m_session, &SessionInterface::focusRequested, this, &Application::focusRequested);
        connect(m_session->surfaceList(), &MirSurfaceListModel::emptyChanged, this, &Application::updateState);

        if (oldFullscreen != fullscreen())
            Q_EMIT fullscreenChanged(fullscreen());

        m_proxySurfaceList->setSourceList(m_session->surfaceList());
        m_proxyPromptSurfaceList->setSourceList(m_session->promptSurfaceList());
    } else {
        // this can only happen after the session has stopped
        Q_ASSERT(m_state == InternalState::Stopped || m_state == InternalState::StoppedResumable
                || m_state == InternalState::Closing);
    }

    Q_EMIT sessionChanged(m_session);
}

void Application::setInternalState(Application::InternalState state)
{
    if (m_state == state) {
        return;
    }

    DEBUG_MSG << "(state=" << internalStateToStr(state) << ")";

    auto oldPublicState = this->state();
    m_state = state;

    switch (m_state) {
        case InternalState::Starting:
        case InternalState::Running:
            acquireWakelock();
            break;
        case InternalState::RunningInBackground:
        case InternalState::Suspended:
            releaseWakelock();
            break;
        case InternalState::Closing:
            Q_EMIT closing();
            acquireWakelock();
            break;
        case InternalState::StoppedResumable:
            releaseWakelock();
            break;
        case InternalState::Stopped:
            Q_EMIT stopped();
            releaseWakelock();
            break;
        case InternalState::SuspendingWaitSession:
        case InternalState::SuspendingWaitProcess:
            // transitory states. leave as it is
        default:
            break;
    };

    if (this->state() != oldPublicState) {
        Q_EMIT stateChanged(this->state());
    }

    updateState();
}

void Application::setProcessState(ProcessState newProcessState)
{
    if (m_processState == newProcessState) {
        return;
    }

    m_processState = newProcessState;

    switch (m_processState) {
    case ProcessUnknown:
        // it would be a coding error
        Q_ASSERT(false);
        break;
    case ProcessRunning:
        if (m_state == InternalState::StoppedResumable) {
            setInternalState(InternalState::Starting);
        }
        break;
    case ProcessSuspended:
        Q_ASSERT(m_state == InternalState::SuspendingWaitProcess);
        setInternalState(InternalState::Suspended);
        break;
    case ProcessFailed:
        // we assume the session always stop before the process
        Q_ASSERT(!m_session || m_session->state() == Session::Stopped);

        if (m_state == InternalState::Starting) {
            // that was way too soon. let it go away
            setInternalState(InternalState::Stopped);
        } else {
            Q_ASSERT(m_state == InternalState::Stopped
                    || m_state == InternalState::StoppedResumable);
        }
        break;
    case ProcessStopped:
        // we assume the session always stop before the process
        Q_ASSERT(!m_session || m_session->state() == Session::Stopped);

        if (m_state == InternalState::Starting) {
            // that was way too soon. let it go away
            setInternalState(InternalState::Stopped);
        } else if (m_state == InternalState::StoppedResumable ||
                   m_state == InternalState::Closing) {
            // The application stopped nicely, likely closed itself. Thus not meant to be resumed later.
            setInternalState(InternalState::Stopped);
        } else {
            Q_ASSERT(m_state == InternalState::Stopped);
        }
        break;
    }

    updateState();
}

void Application::suspend()
{
    DEBUG_MSG << "()";

    Q_ASSERT(m_state == InternalState::Running);
    Q_ASSERT(m_session != nullptr);

    if (exemptFromLifecycle()) {
        // There's no need to keep the wakelock as the process is never suspended
        // and thus has no cleanup to perform when (for example) the display is
        // blanked.
        setInternalState(InternalState::RunningInBackground);
    } else {
        setInternalState(InternalState::SuspendingWaitSession);
        m_session->suspend();
    }
}

void Application::resume()
{
    DEBUG_MSG << "()";

    if (m_state == InternalState::Suspended || m_state == InternalState::SuspendingWaitProcess) {
        Q_EMIT resumeProcessRequested();
        setInternalState(InternalState::Running);
        if (m_processState == ProcessSuspended) {
            setProcessState(ProcessRunning); // should we wait for a resumed() signal?
        }
        if (m_session) {
            m_session->resume();
        }
    } else if (m_state == InternalState::SuspendingWaitSession) {
        setInternalState(InternalState::Running);
        m_session->resume();
    } else if (m_state == InternalState::RunningInBackground) {
        setInternalState(InternalState::Running);
    }
}

void Application::respawn()
{
    DEBUG_MSG << "()";

    setInternalState(InternalState::Starting);

    Q_EMIT startProcessRequested();
}

void Application::stop()
{
    DEBUG_MSG << "()";

    Q_EMIT stopProcessRequested();
}

bool Application::isTouchApp() const
{
    return m_appInfo->isTouchApp();
}

bool Application::exemptFromLifecycle() const
{
    return m_exemptFromLifecycle;
}

void Application::setExemptFromLifecycle(bool exemptFromLifecycle)
{
    if (m_exemptFromLifecycle != exemptFromLifecycle)
    {
        DEBUG_MSG << "(" << exemptFromLifecycle << ")";
        // We don't adjust current suspension state, we only care about exempt
        // status going into a suspend.
        m_exemptFromLifecycle = exemptFromLifecycle;
        Q_EMIT exemptFromLifecycleChanged(m_exemptFromLifecycle);
    }
}

Qt::ScreenOrientations Application::supportedOrientations() const
{
    return m_supportedOrientations;
}

bool Application::rotatesWindowContents() const
{
    return m_rotatesWindowContents;
}

SessionInterface* Application::session() const
{
    return m_session;
}

void Application::acquireWakelock() const
{
    if (appId() == QLatin1String("unity8-dash"))
        return;

    m_sharedWakelock->acquire(this);
}

void Application::releaseWakelock() const
{
    if (appId() == QLatin1String("unity8-dash"))
        return;

    m_sharedWakelock->release(this);
}

void Application::onSessionStateChanged(Session::State sessionState)
{
    switch (sessionState) {
    case Session::Starting:
        break;
    case Session::Running:
        if (m_state == InternalState::Starting) {
            setInternalState(InternalState::Running);
        }
        break;
    case Session::Suspending:
        break;
    case Session::Suspended:
        Q_ASSERT(m_state == InternalState::SuspendingWaitSession);
        setInternalState(InternalState::SuspendingWaitProcess);
        Q_EMIT suspendProcessRequested();
        break;
    case Session::Stopped:
        onSessionStopped();
    }
}

void Application::onSessionStopped()
{
    switch (m_state) {
    case InternalState::Starting:
        /* application has stopped before it managed to create a surface, we can
           assume it crashed on startup, and thus cannot be resumed */
        setInternalState(InternalState::Stopped);
        break;
    case InternalState::Running:
        /* application is on foreground, if Mir reports the application disconnects,
           it either crashed or stopped itself. Either way, it must go away. */
        setInternalState(InternalState::Stopped);
        break;
    case InternalState::RunningInBackground:
        if (m_processState == Application::ProcessFailed) {
            /* killed by the Out-Of-Memory killer while in background. Keep it in the window list
               as the user didn't want it to go away */
            setInternalState(InternalState::StoppedResumable);
        } else {
            /* the application closed itself while in the background. Let it go away */
            setInternalState(InternalState::Stopped);
        }
        break;
    case InternalState::SuspendingWaitSession:
    case InternalState::SuspendingWaitProcess:
        if (m_processState == Application::ProcessFailed) {
            /* killed by the Out-Of-Memory killer while suspended (or getting there), keep it around as the user
               doesn't expect it to disappear */
            setInternalState(InternalState::StoppedResumable);
        } else {
            /* Session stopped normally while we're waiting for suspension */
            stop();
            setInternalState(InternalState::Stopped);
        }
        break;
    case InternalState::Suspended:
        if (m_processState != ProcessUnknown) {
            /* If the user explicly closed this application, we would have it resumed before doing so.
               Since this is not the case, keep it around. */
            setInternalState(InternalState::StoppedResumable);
        } else {
            /* We're not able to respawn this application because it's not managed by upstart
               (probably was launched via cmd line by user) */
            setInternalState(InternalState::Stopped);
        }
        break;
    case InternalState::Closing:
        /* We're expecting the application to stop after a close request */
        setInternalState(InternalState::Stopped);
        break;
    case InternalState::StoppedResumable:
    case InternalState::Stopped:
        /* NOOP */
        break;
    }
}

void Application::setStopTimer(AbstractTimer *timer)
{
    delete m_stopTimer;

    m_stopTimer = timer;
    m_stopTimer->setInterval(1000);
    m_stopTimer->setSingleShot(true);
    connect(m_stopTimer, &Timer::timeout, this, &Application::stop);
}

QSize Application::initialSurfaceSize() const
{
    return m_initialSurfaceSize;
}

void Application::setInitialSurfaceSize(const QSize &size)
{
    DEBUG_MSG << "(size=" << size << ")";

    if (size != m_initialSurfaceSize) {
        m_initialSurfaceSize = size;
        Q_EMIT initialSurfaceSizeChanged(m_initialSurfaceSize);
    }
}

unityapp::MirSurfaceListInterface* Application::surfaceList() const
{
    return m_proxySurfaceList;
}

unityapp::MirSurfaceListInterface* Application::promptSurfaceList() const
{
    return m_proxyPromptSurfaceList;
}

void Application::requestFocus()
{
    if (m_proxySurfaceList->rowCount() > 0) {
        DEBUG_MSG << "() - Requesting focus for most recent app surface";
        m_proxySurfaceList->get(0)->requestFocus();
    } else {
        DEBUG_MSG << "() - emitting focusRequested()";
        Q_EMIT focusRequested();
    }
}

} // namespace qtmir
