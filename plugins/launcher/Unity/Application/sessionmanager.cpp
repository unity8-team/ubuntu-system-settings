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

// Qt
#include <QGuiApplication>

// local
#include "application_manager.h"
#include "debughelpers.h"
#include "sessionmanager.h"

// QPA mirserver
#include "nativeinterface.h"
#include "sessionlistener.h"
#include "logging.h"
#include "promptsessionlistener.h"

// mir
#include <mir/scene/prompt_session.h>
#include <mir/scene/prompt_session_manager.h>
#include <mir/report_exception.h>

namespace ms = mir::scene;

namespace qtmir {

SessionManager *SessionManager::the_session_manager = nullptr;


void connectToSessionListener(SessionManager *manager, SessionListener *listener)
{
    QObject::connect(listener, &SessionListener::sessionStarting,
                     manager, &SessionManager::onSessionStarting);
    QObject::connect(listener, &SessionListener::sessionStopping,
                     manager, &SessionManager::onSessionStopping);
}

void connectToPromptSessionListener(SessionManager * manager, PromptSessionListener * listener)
{
    QObject::connect(listener, &PromptSessionListener::promptSessionStarting,
                     manager, &SessionManager::onPromptSessionStarting);
    QObject::connect(listener, &PromptSessionListener::promptSessionStopping,
                     manager, &SessionManager::onPromptSessionStopping);
    QObject::connect(listener, &PromptSessionListener::promptProviderAdded,
                     manager, &SessionManager::onPromptProviderAdded);
    QObject::connect(listener, &PromptSessionListener::promptProviderRemoved,
                     manager, &SessionManager::onPromptProviderRemoved);
}

SessionManager* SessionManager::singleton()
try
{
    if (!the_session_manager) {

        NativeInterface *nativeInterface = dynamic_cast<NativeInterface*>(QGuiApplication::platformNativeInterface());

        if (!nativeInterface) {
            qCritical("ERROR: Unity.Application QML plugin requires use of the 'mirserver' QPA plugin");
            QGuiApplication::quit();
            return nullptr;
        }

        SessionListener *sessionListener = static_cast<SessionListener*>(nativeInterface->nativeResourceForIntegration("SessionListener"));
        PromptSessionListener *promptSessionListener = static_cast<PromptSessionListener*>(nativeInterface->nativeResourceForIntegration("PromptSessionListener"));

        the_session_manager = new SessionManager(nativeInterface->thePromptSessionManager(), ApplicationManager::singleton());

        connectToSessionListener(the_session_manager, sessionListener);
        connectToPromptSessionListener(the_session_manager, promptSessionListener);
    }
    return the_session_manager;
}
catch (...)
{
    // We only call mir::report_exception() here to force linkage against libmirserver.
    // Unless we force this module to have a link dependency on libmirserver we get
    // several tests hanging during link loading. I wish I understood why.    alan_g
    mir::report_exception();
    throw;
}

SessionManager::SessionManager(
        const std::shared_ptr<mir::scene::PromptSessionManager>& promptSessionManager,
        ApplicationManager* applicationManager,
        QObject *parent)
    : SessionModel(parent)
    , m_promptSessionManager(promptSessionManager)
    , m_applicationManager(applicationManager)
{
    qCDebug(QTMIR_SESSIONS) << "SessionManager::SessionManager - this=" << this;
    setObjectName(QStringLiteral("qtmir::SessionManager"));
}

SessionManager::~SessionManager()
{
    qCDebug(QTMIR_SESSIONS) << "SessionManager::~SessionManager - this=" << this;
}

SessionInterface *SessionManager::findSession(const mir::scene::Session* session) const
{
    if (!session) return nullptr;

    for (SessionInterface* child : list()) {
        if (child->session().get() == session)
            return child;
    }
    return nullptr;
}

void SessionManager::onSessionStarting(std::shared_ptr<mir::scene::Session> const& session)
{
    qCDebug(QTMIR_SESSIONS) << "SessionManager::onSessionStarting - sessionName=" <<  session->name().c_str();

    Session* qmlSession = new Session(session, m_promptSessionManager);
    insert(0, qmlSession);

    Application* application = m_applicationManager->findApplicationWithSession(session);
    if (application && application->state() != Application::Running) {
        application->setSession(qmlSession);
    }
    // need to remove if we've destroyed outside
    connect(qmlSession, &Session::destroyed, this, [&](QObject *item) {
        auto sessionToRemove = static_cast<Session*>(item);
        remove(sessionToRemove);
    });

    Q_EMIT sessionStarting(qmlSession);
}

void SessionManager::onSessionStopping(std::shared_ptr<mir::scene::Session> const& session)
{
    qCDebug(QTMIR_SESSIONS) << "SessionManager::onSessionStopping - sessionName=" << session->name().c_str();

    SessionInterface* qmlSession = findSession(session.get());
    if (!qmlSession) return;

    remove(qmlSession);

    qmlSession->setLive(false);
    Q_EMIT sessionStopping(qmlSession);
}

void SessionManager::onPromptSessionStarting(const std::shared_ptr<ms::PromptSession>& promptSession)
{
    qCDebug(QTMIR_SESSIONS) << "SessionManager::onPromptSessionStarting - promptSession=" << promptSession.get();

    std::shared_ptr<mir::scene::Session> appSession = m_promptSessionManager->application_for(promptSession);
    SessionInterface *qmlAppSession = findSession(appSession.get());
    if (qmlAppSession) {
        m_mirPromptToSessionHash[promptSession.get()] = qmlAppSession;
        qmlAppSession->appendPromptSession(promptSession);
    } else {
        qCDebug(QTMIR_SESSIONS) << "SessionManager::onPromptSessionStarting - could not find app session for prompt session";
    }
}

void SessionManager::onPromptSessionStopping(const std::shared_ptr<ms::PromptSession>& promptSession)
{
    qCDebug(QTMIR_SESSIONS) << "SessionManager::onPromptSessionStopping - promptSession=" << promptSession.get();

    for (SessionInterface *qmlSession : this->list()) {
        qmlSession->removePromptSession(promptSession);
    }
    m_mirPromptToSessionHash.remove(promptSession.get());
}

void SessionManager::onPromptProviderAdded(const mir::scene::PromptSession *promptSession,
                                              const std::shared_ptr<mir::scene::Session> &promptProvider)
{
    qCDebug(QTMIR_SESSIONS) << "SessionManager::onPromptProviderAdded - promptSession=" << promptSession << " promptProvider=" << promptProvider.get();

    SessionInterface* qmlAppSession = m_mirPromptToSessionHash.value(promptSession, nullptr);
    if (!qmlAppSession) {
        qCDebug(QTMIR_SESSIONS) << "SessionManager::onPromptProviderAdded - could not find session item for app session";
        return;
    }

    SessionInterface* qmlPromptProvider = findSession(promptProvider.get());
    if (!qmlPromptProvider) {
        qCDebug(QTMIR_SESSIONS) << "SessionManager::onPromptProviderAdded - could not find session item for provider session";
        return;
    }

    qmlAppSession->addChildSession(qmlPromptProvider);
}

void SessionManager::onPromptProviderRemoved(const mir::scene::PromptSession *promptSession,
                                                const std::shared_ptr<mir::scene::Session> &promptProvider)
{
    qCDebug(QTMIR_SESSIONS) << "SessionManager::onPromptProviderRemoved - promptSession=" << promptSession << " promptProvider=" << promptProvider.get();

    SessionInterface* qmlPromptProvider = findSession(promptProvider.get());
    if (!qmlPromptProvider) {
        qCDebug(QTMIR_SESSIONS) << "SessionManager::onPromptProviderAdded - could not find session item for provider session";
        return;
    }
    qmlPromptProvider->setLive(false);
}

} // namespace qtmir
