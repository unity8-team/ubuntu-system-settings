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

#ifndef APPLICATION_H
#define APPLICATION_H

// std
#include <memory>

//Qt
#include <QtCore/QtCore>
#include <QImage>
#include <QSharedPointer>

// Unity API
#include <unity/shell/application/ApplicationInfoInterface.h>

#include "mirsurfacelistmodel.h"
#include "session_interface.h"

namespace mir {
    namespace scene {
        class Session;
    }
}

namespace qtmir
{

class ApplicationManager;
class ApplicationInfo;
class Session;
class SharedWakelock;
class AbstractTimer;

class Application : public unity::shell::application::ApplicationInfoInterface
{
    Q_OBJECT

    Q_PROPERTY(bool fullscreen READ fullscreen NOTIFY fullscreenChanged)

public:
    Q_DECLARE_FLAGS(Stages, Stage)

    enum ProcessState {
        ProcessUnknown, // not managed by upstart, so we can't respawn that application
        ProcessRunning,
        ProcessSuspended,
        ProcessFailed, // it stopped, but because it was killed or because it crashed
        ProcessStopped
    };

    enum class InternalState {
        Starting,
        Running,
        RunningInBackground,
        SuspendingWaitSession,
        SuspendingWaitProcess,
        Suspended,
        Closing, // The user has requested the app be closed
        StoppedResumable, // The process stopped but we want to keep the Application object around
                          // so it can be respawned as if it never stopped running in the first place.
        Stopped // It closed itself, crashed or it stopped and we can't respawn it
                // In any case, this is a dead end. The Application object can be deleted at
                // any moment once in this state.
    };

    Application(const QSharedPointer<SharedWakelock>& sharedWakelock,
                const QSharedPointer<ApplicationInfo>& appInfo,
                const QStringList &arguments = QStringList(),
                ApplicationManager *parent = nullptr);
    virtual ~Application();

    // ApplicationInfoInterface
    QString appId() const override;
    QString name() const override;
    QString comment() const override;
    QUrl icon() const override;
    State state() const override;
    RequestedState requestedState() const override;
    void setRequestedState(RequestedState) override;
    bool focused() const override;
    QString splashTitle() const override;
    QUrl splashImage() const override;
    bool splashShowHeader() const override;
    QColor splashColor() const override;
    QColor splashColorHeader() const override;
    QColor splashColorFooter() const override;
    Qt::ScreenOrientations supportedOrientations() const override;
    bool rotatesWindowContents() const override;
    bool isTouchApp() const override;
    bool exemptFromLifecycle() const override;
    void setExemptFromLifecycle(bool) override;
    QSize initialSurfaceSize() const override;
    void setInitialSurfaceSize(const QSize &size) override;
    unity::shell::application::MirSurfaceListInterface* surfaceList() const override;
    unity::shell::application::MirSurfaceListInterface* promptSurfaceList() const override;
    int surfaceCount() const override { return surfaceList()->count(); }

    ProcessState processState() const { return m_processState; }
    void setProcessState(ProcessState value);

    QStringList arguments() const { return m_arguments; }

    SessionInterface* session() const;
    void setSession(SessionInterface *session);

    bool isValid() const;
    bool fullscreen() const;

    Stages supportedStages() const;

    pid_t pid() const;

    void close();

    // internal as in "not exposed in unity-api", so qtmir-internal.
    InternalState internalState() const { return m_state; }

    void requestFocus();

    // for tests
    void setStopTimer(AbstractTimer *timer);
    AbstractTimer *stopTimer() const { return m_stopTimer; }

Q_SIGNALS:
    void fullscreenChanged(bool fullscreen);
    void sessionChanged(SessionInterface *session);

    void startProcessRequested();
    void stopProcessRequested();
    void suspendProcessRequested();
    void resumeProcessRequested();
    void stopped();
    void closing();

private Q_SLOTS:
    void onSessionStateChanged(SessionInterface::State sessionState);

    void respawn();

private:

    void acquireWakelock() const;
    void releaseWakelock() const;
    void setPid(pid_t pid);
    void setArguments(const QStringList &arguments);
    void setInternalState(InternalState state);
    void wipeQMLCache();
    void suspend();
    void resume();
    void stop();
    QColor colorFromString(const QString &colorString, const char *colorName) const;
    static const char* internalStateToStr(InternalState state);
    void updateState();
    void applyRequestedRunning();
    void applyRequestedSuspended();
    void applyClosing();
    void onSessionStopped();

    QSharedPointer<SharedWakelock> m_sharedWakelock;
    QSharedPointer<ApplicationInfo> m_appInfo;
    pid_t m_pid;
    Stages m_supportedStages;
    InternalState m_state;
    QStringList m_arguments;
    Qt::ScreenOrientations m_supportedOrientations;
    bool m_rotatesWindowContents;
    SessionInterface *m_session;
    RequestedState m_requestedState;
    ProcessState m_processState;
    AbstractTimer *m_stopTimer;
    bool m_exemptFromLifecycle;
    QSize m_initialSurfaceSize;
    bool m_closing{false};

    ProxySurfaceListModel *m_proxySurfaceList;
    ProxySurfaceListModel *m_proxyPromptSurfaceList;

    friend class ApplicationManager;
    friend class SessionManager;
    friend class Session;
};

} // namespace qtmir

Q_DECLARE_METATYPE(qtmir::Application*)

#endif  // APPLICATION_H
