/*
 * Copyright (C) 2014-2015 Canonical, Ltd.
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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

// std
#include <memory>

// Qt
#include <QHash>
#include <QSharedPointer>

// Mir
#include <mir_toolkit/common.h>

// local
#include "session.h"
#include "sessionmodel.h"

namespace mir {
    namespace scene {
        class Session;
        class PromptSession;
    }
}

namespace qtmir {

class Application;
class ApplicationManager;

class SessionManager : public SessionModel
{
    Q_OBJECT

public:
    explicit SessionManager(
        const std::shared_ptr<mir::scene::PromptSessionManager>& promptSessionManager,
        ApplicationManager* applicationManager,
        QObject *parent = 0
    );
    ~SessionManager();

    static SessionManager* singleton();

    SessionInterface *findSession(const mir::scene::Session* session) const;

Q_SIGNALS:
    void sessionStarting(SessionInterface* session);
    void sessionStopping(SessionInterface* session);

public Q_SLOTS:
    void onSessionStarting(std::shared_ptr<mir::scene::Session> const& session);
    void onSessionStopping(std::shared_ptr<mir::scene::Session> const& session);

    void onPromptSessionStarting(const std::shared_ptr<mir::scene::PromptSession>& promptSession);
    void onPromptSessionStopping(const std::shared_ptr<mir::scene::PromptSession>& promptSession);
    void onPromptProviderAdded(const mir::scene::PromptSession *, const std::shared_ptr<mir::scene::Session> &);
    void onPromptProviderRemoved(const mir::scene::PromptSession *, const std::shared_ptr<mir::scene::Session> &);

protected:

private:
    const std::shared_ptr<mir::scene::PromptSessionManager> m_promptSessionManager;
    ApplicationManager* m_applicationManager;
    static SessionManager *the_session_manager;

    QHash<const mir::scene::PromptSession *, SessionInterface *> m_mirPromptToSessionHash;
};

} // namespace qtmir

#endif // SESSIONMANAGER_H
