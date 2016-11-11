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
#include "application_manager.h"
#include "application.h"
#include "applicationinfo.h"
#include "dbusfocusinfo.h"
#include "mirfocuscontroller.h"
#include "session.h"
#include "sharedwakelock.h"
#include "proc_info.h"
#include "upstart/taskcontroller.h"
#include "tracepoints.h" // generated from tracepoints.tp
#include "settings.h"

// mirserver
#include "nativeinterface.h"
#include "sessionauthorizer.h"
#include "logging.h"
#include <mirwindowmanager.h>

// mir
#include <mir/scene/surface.h>
#include <mir/graphics/display.h>
#include <mir/graphics/display_buffer.h>
#include <mir/geometry/rectangles.h>

// Qt
#include <QGuiApplication>
#include <QDebug>
#include <QByteArray>
#include <QDir>

// std
#include <csignal>

// Unity API
#include <unity/shell/application/MirSurfaceInterface.h>

namespace ms = mir::scene;

namespace unityapi = unity::shell::application;

#define DEBUG_MSG qCDebug(QTMIR_APPLICATIONS).nospace() << "ApplicationManager::" << __func__

namespace qtmir
{

namespace {

// FIXME: To be removed once shell has fully adopted short appIds!!
QString toShortAppIdIfPossible(const QString &appId) {
    QRegExp longAppIdMask(QStringLiteral("[a-z0-9][a-z0-9+.-]+_[a-zA-Z0-9+.-]+_[0-9][a-zA-Z0-9.+:~-]*"));
    if (longAppIdMask.exactMatch(appId)) {
        qWarning() << "WARNING: long App ID encountered:" << appId;
        // input string a long AppId, chop the version string off the end
        QStringList parts = appId.split(QStringLiteral("_"));
        return QStringLiteral("%1_%2").arg(parts.first(), parts.at(1));
    }
    return appId;
}

void connectToSessionAuthorizer(ApplicationManager *manager, SessionAuthorizer *authorizer)
{
    QObject::connect(authorizer, &SessionAuthorizer::requestAuthorizationForSession,
                     manager, &ApplicationManager::authorizeSession, Qt::BlockingQueuedConnection);
}

void connectToTaskController(ApplicationManager *manager, TaskController *controller)
{
    // TaskController::processStarting blocks Ubuntu-App-Launch from executing the process, have it return
    // as fast as possible! Using a Queued connection will push an event on the event queue before the
    // (blocking) event for authorizeSession is pushed on the same queue - so the application's processState
    // will be up-to-date when authorizeSession is called.
    //
    // TODO: Unfortunately making this queued unearths a crash (likely in Qt) (LP: #1616842).
    QObject::connect(controller, &TaskController::processStarting,
                     manager, &ApplicationManager::onProcessStarting);

    QObject::connect(controller, &TaskController::processStopped,
                     manager, &ApplicationManager::onProcessStopped);
    QObject::connect(controller, &TaskController::processSuspended,
                     manager, &ApplicationManager::onProcessSuspended);
    QObject::connect(controller, &TaskController::processFailed,
                     manager, &ApplicationManager::onProcessFailed);
    QObject::connect(controller, &TaskController::focusRequested,
                     manager, &ApplicationManager::onFocusRequested);
    QObject::connect(controller, &TaskController::resumeRequested,
                     manager, &ApplicationManager::onResumeRequested);
}

} // namespace

ApplicationManager* ApplicationManager::Factory::Factory::create()
{
    NativeInterface *nativeInterface = dynamic_cast<NativeInterface*>(QGuiApplication::platformNativeInterface());

    if (!nativeInterface) {
        qCritical() << "ERROR: Unity.Application QML plugin requires use of the 'mirserver' QPA plugin";
        QGuiApplication::quit();
        return nullptr;
    }

    MirWindowManager *windowManager =  static_cast<MirWindowManager*>(nativeInterface->nativeResourceForIntegration("WindowManager"));
    SessionAuthorizer *sessionAuthorizer = static_cast<SessionAuthorizer*>(nativeInterface->nativeResourceForIntegration("SessionAuthorizer"));

    QSharedPointer<TaskController> taskController(new upstart::TaskController());
    QSharedPointer<ProcInfo> procInfo(new ProcInfo());
    QSharedPointer<SharedWakelock> sharedWakelock(new SharedWakelock);
    QSharedPointer<Settings> settings(new Settings());

    // FIXME: We should use a QSharedPointer to wrap this ApplicationManager object, which requires us
    // to use the data() method to pass the raw pointer to the QML engine. However the QML engine appears
    // to take ownership of the object, and deletes it when it wants to. This conflicts with the purpose
    // of the QSharedPointer, and a double-delete results. Trying QQmlEngine::setObjectOwnership on the
    // object no effect, which it should. Need to investigate why.
    ApplicationManager* appManager = new ApplicationManager(
                                             taskController,
                                             sharedWakelock,
                                             procInfo,
                                             settings
                                         );

    connectToSessionAuthorizer(appManager, sessionAuthorizer);
    connectToTaskController(appManager, taskController.data());
    connect(windowManager, &MirWindowManager::sessionAboutToCreateSurface,
            appManager, &ApplicationManager::onSessionAboutToCreateSurface,
            Qt::BlockingQueuedConnection);

    // Emit signal to notify Upstart that Mir is ready to receive client connections
    // see http://upstart.ubuntu.com/cookbook/#expect-stop
    // FIXME: should not be qtmir's job, instead should notify the user of this library
    // that they should emit this signal, perhaps by posting an event to the
    // QMirServerApplication event loop when it comes up
    if (qgetenv("UNITY_MIR_EMITS_SIGSTOP") == "1") {
        raise(SIGSTOP);
    }

    return appManager;
}


ApplicationManager* ApplicationManager::singleton()
{
    static ApplicationManager* instance;
    if (!instance) {
        Factory appFactory;
        instance = appFactory.create();
    }
    return instance;
}

ApplicationManager::ApplicationManager(
        const QSharedPointer<TaskController>& taskController,
        const QSharedPointer<SharedWakelock>& sharedWakelock,
        const QSharedPointer<ProcInfo>& procInfo,
        const QSharedPointer<SettingsInterface>& settings,
        QObject *parent)
    : ApplicationManagerInterface(parent)
    , m_dbusFocusInfo(new DBusFocusInfo(m_applications))
    , m_taskController(taskController)
    , m_procInfo(procInfo)
    , m_sharedWakelock(sharedWakelock)
    , m_settings(settings)
{
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::ApplicationManager (this=%p)" << this;
    setObjectName(QStringLiteral("qtmir::ApplicationManager"));

    /*
        All begin[...]Rows() and end[...]Rows() functions cause signal emissions which can
        be processed by slots immediately and then trigger yet more model changes.

        The connection below is queued to avoid stacked model change attempts cause by the above,
        such as attempting to raise the newly focused application while another one is still
        getting removed from the model.
     */
    connect(MirFocusController::instance(), &MirFocusController::focusedSurfaceChanged,
        this, &ApplicationManager::updateFocusedApplication, Qt::QueuedConnection);
}

ApplicationManager::~ApplicationManager()
{
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::~ApplicationManager";
    delete m_dbusFocusInfo;
}

int ApplicationManager::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_applications.size() : 0;
}

QVariant ApplicationManager::data(const QModelIndex &index, int role) const
{
    if (index.row() >= 0 && index.row() < m_applications.size()) {
        Application *application = m_applications.at(index.row());
        switch (role) {
            case RoleAppId:
                return QVariant::fromValue(application->appId());
            case RoleName:
                return QVariant::fromValue(application->name());
            case RoleComment:
                return QVariant::fromValue(application->comment());
            case RoleIcon:
                return QVariant::fromValue(application->icon());
            case RoleState:
                return QVariant::fromValue((int)application->state());
            case RoleFocused:
                return QVariant::fromValue(application->focused());
            case RoleIsTouchApp:
                return QVariant::fromValue(application->isTouchApp());
            case RoleExemptFromLifecycle:
                return QVariant::fromValue(application->exemptFromLifecycle());
            case RoleApplication:
                return QVariant::fromValue(application);
            default:
                return QVariant();
        }
    } else {
        return QVariant();
    }
}

Application* ApplicationManager::get(int index) const
{
    if (index < 0 || index >= m_applications.count()) {
        return nullptr;
    }
    return m_applications.at(index);
}

Application* ApplicationManager::findApplication(const QString &inputAppId) const
{
    const QString appId = toShortAppIdIfPossible(inputAppId);

    for (Application *app : m_applications) {
        if (app->appId() == appId) {
            return app;
        }
    }
    return nullptr;
}

bool ApplicationManager::requestFocusApplication(const QString &inputAppId)
{
    const QString appId = toShortAppIdIfPossible(inputAppId);

    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::requestFocusApplication - appId=" << appId;
    Application *application = findApplication(appId);

    if (!application) {
        qDebug() << "No such running application with appId=" << appId;
        return false;
    }

    application->requestFocus();

    return true;
}

QString ApplicationManager::focusedApplicationId() const
{
    Application *focusedApplication = nullptr;
    auto surface = static_cast<qtmir::MirSurfaceInterface*>(MirFocusController::instance()->focusedSurface());
    if (surface) {
        auto self = const_cast<ApplicationManager*>(this);
        focusedApplication = self->findApplication(surface);
    }

    if (focusedApplication) {
        return focusedApplication->appId();
    } else {
        return QString();
    }
}

/**
 * @brief ApplicationManager::startApplication launches an application identified by an "application id" or appId.
 *
 * Note: due to an implementation detail, appIds come in two forms:
 * * long appId: $(click_package)_$(application)_$(version)
 * * short appId: $(click_package)_$(application)
 * It is expected that the shell uses _only_ short appIds (but long appIds are accepted by this method for legacy
 * reasons - but be warned, this ability will be removed)
 *
 * Unless stated otherwise, we always use short appIds in this API.
 *
 * @param inputAppId AppId of application to launch (long appId supported)
 * @param arguments Command line arguments to pass to the application to be launched
 * @return Pointer to Application object representing the launched process. If process already running, return nullptr
 */
Application* ApplicationManager::startApplication(const QString &inputAppId,
                                                  const QStringList &arguments)
{
    tracepoint(qtmir, startApplication);
    QString appId = toShortAppIdIfPossible(inputAppId);
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::startApplication - this=" << this << "appId" << qPrintable(appId);

    Application *application = findApplication(appId);
    if (application) {
        qWarning() << "ApplicationManager::startApplication - application appId=" << appId << " already exists";
        return nullptr;
    }

    if (m_queuedStartApplications.contains(inputAppId)) {
        qWarning() << "ApplicationManager::startApplication - application appId=" << appId << " is queued to start";
        return nullptr;
    } else {
        application = findClosingApplication(inputAppId);
        if (application) {
            m_queuedStartApplications.append(inputAppId);
            qWarning() << "ApplicationManager::startApplication - application appId=" << appId << " is closing. Queuing start";
            connect(application, &QObject::destroyed, this, [this, application, inputAppId, arguments]() {
                m_queuedStartApplications.removeAll(inputAppId);
                // start the app.
                startApplication(inputAppId, arguments);
            }, Qt::QueuedConnection); // Queued so that we finish the app removal before starting again.
            return nullptr;
        }
    }

    if (!m_taskController->start(appId, arguments)) {
        qWarning() << "Upstart failed to start application with appId" << appId;
        return nullptr;
    }

    // The TaskController may synchroneously callback onProcessStarting, so check if application already added
    application = findApplication(appId);
    if (application) {
        application->setArguments(arguments);
    } else {
        auto appInfo = m_taskController->getInfoForApp(appId);
        if (!appInfo) {
            qCWarning(QTMIR_APPLICATIONS) << "ApplicationManager::startApplication - Unable to instantiate application with appId" << appId;
            return nullptr;
        }

        application = new Application(
                    m_sharedWakelock,
                    appInfo,
                    arguments,
                    this);

        add(application);
    }
    return application;
}

void ApplicationManager::onProcessStarting(const QString &appId)
{
    tracepoint(qtmir, onProcessStarting);
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::onProcessStarting - appId=" << appId;

    Application *application = findApplication(appId);
    if (!application) { // then shell did not start this application, so ubuntu-app-launch must have - add to list
        auto appInfo = m_taskController->getInfoForApp(appId);
        if (!appInfo) {
            qCWarning(QTMIR_APPLICATIONS) << "ApplicationManager::onProcessStarting - Unable to instantiate application with appId" << appId;
            return;
        }

        application = new Application(
                    m_sharedWakelock,
                    appInfo,
                    QStringList(),
                    this);
        add(application);
        application->requestFocus();
    }
    else {
        if (application->internalState() == Application::InternalState::StoppedResumable) {
            // url-dispatcher can relaunch apps which have been OOM-killed - AppMan must accept the newly spawned
            // application and focus it immediately (as user expects app to still be running).
            qCDebug(QTMIR_APPLICATIONS) << "Stopped application appId=" << appId << "is being resumed externally";
            application->requestFocus();
        } else {
            qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::onProcessStarting application already found with appId"
                                        << appId;
        }
    }
    application->setProcessState(Application::ProcessRunning);
}

/**
 * @brief ApplicationManager::stopApplication - stop a running application and remove from list
 * @param inputAppId
 * @return True if running application was stopped, false if application did not exist or could not be stopped
 */
bool ApplicationManager::stopApplication(const QString &inputAppId)
{
    const QString appId = toShortAppIdIfPossible(inputAppId);
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::stopApplication - appId=" << appId;

    Application *application = findApplication(appId);
    if (!application) {
        qCritical() << "No such running application with appId" << appId;
        return false;
    }

    application->close();

    return true;
}

void ApplicationManager::onApplicationClosing(Application *application)
{
    remove(application);

    connect(application, &QObject::destroyed, this, [this, application](QObject*) {
        m_closingApplications.removeAll(application);
    });
    m_closingApplications.append(application);
}

void ApplicationManager::onProcessFailed(const QString &appId, TaskController::Error error)
{
    // Applications fail if they fail to launch, crash or are killed.

    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::onProcessFailed - appId=" << appId;

    Application *application = findApplication(appId);
    if (!application) {
        qWarning() << "ApplicationManager::onProcessFailed - upstart reports failure of application" << appId
                   << "that AppManager is not managing";
        return;
    }

    Q_UNUSED(error); // FIXME(greyback) upstart reports app that fully started up & crashes as failing during startup??
    application->setProcessState(Application::ProcessFailed);
    application->setPid(0);
}

void ApplicationManager::onProcessStopped(const QString &appId)
{
    tracepoint(qtmir, onProcessStopped);
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::onProcessStopped - appId=" << appId;

    Application *application = findApplication(appId);
    if (!application) {
        application = findClosingApplication(appId);
    }

    if (!application) {
        qDebug() << "ApplicationManager::onProcessStopped reports stop of appId=" << appId
                 << "which AppMan is not managing, ignoring the event";
        return;
    }

    // if an application gets killed, onProcessFailed is called first, followed by onProcessStopped.
    // we don't want to override what onProcessFailed already set.
    if (application->processState() != Application::ProcessFailed) {
        application->setProcessState(Application::ProcessStopped);
        application->setPid(0);
    }
}

void ApplicationManager::onProcessSuspended(const QString &appId)
{
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::onProcessSuspended - appId=" << appId;

    Application *application = findApplication(appId);

    if (!application) {
        qDebug() << "ApplicationManager::onProcessSuspended reports stop of appId=" << appId
                 << "which AppMan is not managing, ignoring the event";
        return;
    }

    application->setProcessState(Application::ProcessSuspended);
}

void ApplicationManager::onFocusRequested(const QString& appId)
{
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::onFocusRequested - appId=" << appId;

    Application *application = findApplication(appId);
    if (application) {
        application->requestFocus();
    }
}

void ApplicationManager::onResumeRequested(const QString& appId)
{
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::onResumeRequested - appId=" << appId;

    Application *application = findApplication(appId);

    if (!application) {
        qCritical() << "ApplicationManager::onResumeRequested: No such running application" << appId;
        return;
    }

    // We interpret this as a focus request for a suspended app.
    // Shell will have this app resumed if it complies with the focus request
    if (application->state() == Application::Suspended) {
        application->requestFocus();
    }
}

void ApplicationManager::onAppDataChanged(const int role)
{
    if (sender()) {
        Application *application = static_cast<Application*>(sender());
        QModelIndex appIndex = findIndex(application);
        Q_EMIT dataChanged(appIndex, appIndex, QVector<int>() << role);
    }
}

void ApplicationManager::authorizeSession(const pid_t pid, bool &authorized)
{
    tracepoint(qtmir, authorizeSession);
    authorized = false; //to be proven wrong

    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::authorizeSession - pid=" << pid;

    Q_FOREACH (Application *app, m_applications) {
        if (app->state() == Application::Starting) {
            tracepoint(qtmir, appIdHasProcessId_start);
            if (m_taskController->appIdHasProcessId(app->appId(), pid)) {
                app->setPid(pid);
                authorized = true;
                tracepoint(qtmir, appIdHasProcessId_end, 1); //found
                return;
            }
            tracepoint(qtmir, appIdHasProcessId_end, 0); // not found
        }
    }

    /*
     * Hack: Allow applications to be launched without being managed by upstart, where AppManager
     * itself manages processes executed with a "--desktop_file_hint=/path/to/desktopFile.desktop"
     * parameter attached. This exists until ubuntu-app-launch can notify shell any application is
     * and so shell should allow it.
     */
    std::unique_ptr<ProcInfo::CommandLine> info = m_procInfo->commandLine(pid);
    if (!info) {
        qWarning() << "ApplicationManager REJECTED connection from app with pid" << pid
                   << "as unable to read the process command line";
        return;
    }

    if (info->startsWith("maliit-server") || info->contains("qt5/libexec/QtWebProcess")) {
        authorized = true;
        return;
    }

    const QString desktopFileName = info->getParameter("--desktop_file_hint=");

    if (desktopFileName.isNull()) {
        qCritical() << "ApplicationManager REJECTED connection from app with pid" << pid
                    << "as it was not launched by upstart, and no desktop_file_hint is specified";
        return;
    }

    // Guess appId from the desktop file hint
    const QString appId = toShortAppIdIfPossible(desktopFileName.split('/').last().remove(QRegExp(QStringLiteral(".desktop$"))));

    qCDebug(QTMIR_APPLICATIONS) << "Process supplied desktop_file_hint, loading:" << appId;

    auto appInfo = m_taskController->getInfoForApp(appId);
    if (!appInfo) {
        qCritical() << "ApplicationManager REJECTED connection from app with pid" << pid
                    << "as the app specified by the desktop_file_hint argument could not be found";
        return;
    }

    // some naughty applications use a script to launch the actual application. Check for the
    // case where shell actually launched the script.
    Application *application = findApplication(appInfo->appId());
    if (application && application->state() == Application::Starting) {
        qCDebug(QTMIR_APPLICATIONS) << "Process with pid" << pid << "appeared, attaching to existing entry"
                                    << "in application list with appId:" << application->appId();
        application->setPid(pid);
        authorized = true;
        return;
    }

    qCDebug(QTMIR_APPLICATIONS) << "New process with pid" << pid << "appeared, adding new application to the"
                                << "application list with appId:" << appInfo->appId();

    QStringList arguments(info->asStringList());
    application = new Application(
        m_sharedWakelock,
        appInfo,
        arguments,
        this);
    application->setPid(pid);
    add(application);
    authorized = true;
}

Application* ApplicationManager::findApplicationWithSession(const std::shared_ptr<ms::Session> &session)
{
    return findApplicationWithSession(session.get());
}

Application* ApplicationManager::findApplicationWithSession(const ms::Session *session)
{
    if (!session)
        return nullptr;
    return findApplicationWithPid(session->process_id());
}

Application* ApplicationManager::findApplicationWithPid(const pid_t pid) const
{
    if (pid <= 0)
        return nullptr;

    for (Application *app : m_applications) {
        if (app->m_pid == pid) {
            return app;
        }
    }
    return nullptr;
}

void ApplicationManager::add(Application* application)
{
    Q_ASSERT(application != nullptr);

    if (m_applications.indexOf(application) != -1) {
        DEBUG_MSG << "(appId=" << application->appId() << ") - already exists";
        return;
    }
    DEBUG_MSG << "(appId=" << application->appId() << ")";

    Q_ASSERT(!m_modelUnderChange);
    m_modelUnderChange = true;

    connect(application, &Application::focusedChanged, this, [this](bool) { onAppDataChanged(RoleFocused); });
    connect(application, &Application::stateChanged, this, [this](Application::State) { onAppDataChanged(RoleState); });
    connect(application, &Application::closing, this, [this, application]() { onApplicationClosing(application); });
    connect(application, &unityapi::ApplicationInfoInterface::focusRequested, this, [this, application]() {
        Q_EMIT focusRequested(application->appId());
    });

    QString appId = application->appId();
    QStringList arguments = application->arguments();

    // The connection is queued as a workaround an issue in the PhoneStage animation that
    // happens when you tap on a killed app in the spread to bring it to foreground, causing
    // a Application::respawn() to take place.
    // In any case, it seems like in general QML works better when don't do too many things
    // in the same event loop iteration.
    connect(application, &Application::startProcessRequested,
            this, [=]() { m_taskController->start(appId, arguments); },
            Qt::QueuedConnection);

    connect(application, &Application::stopProcessRequested, this, [=]() {
        if (!m_taskController->stop(appId) && application->pid() > 0) {
            qWarning() << "FAILED to ask Upstart to stop application with appId" << appId
                       << "Sending SIGTERM to process:" << appId;
            kill(application->pid(), SIGTERM);
            application->setProcessState(Application::ProcessStopped);
        }
    });

    connect(application, &Application::suspendProcessRequested, this, [=]() { m_taskController->suspend(appId); } );
    connect(application, &Application::resumeProcessRequested, this, [=]() { m_taskController->resume(appId); } );

    connect(application, &Application::stopped, this, [=]() {
        remove(application);
        application->deleteLater();
    });


    beginInsertRows(QModelIndex(), m_applications.count(), m_applications.count());
    m_applications.append(application);
    endInsertRows();
    Q_EMIT countChanged();
    if (m_applications.size() == 1) {
        Q_EMIT emptyChanged();
    }

    m_modelUnderChange = false;

    DEBUG_MSG << "(appId=" << application->appId() << ") - after " << toString();
}

void ApplicationManager::remove(Application *application)
{
    Q_ASSERT(application != nullptr);

    int index = m_applications.indexOf(application);
    if (index == -1) {
        DEBUG_MSG << "(appId=" << application->appId() << ") - not found";
        return;
    }

    DEBUG_MSG << "(appId=" << application->appId() << ") - before " << toString();

    Q_ASSERT(!m_modelUnderChange);
    m_modelUnderChange = true;

    beginRemoveRows(QModelIndex(), index, index);
    m_applications.removeAt(index);
    endRemoveRows();
    Q_EMIT countChanged();
    if (index == 0) {
        Q_EMIT emptyChanged();
    }

    disconnect(application, &Application::fullscreenChanged, this, 0);
    disconnect(application, &Application::focusedChanged, this, 0);
    disconnect(application, &Application::stateChanged, this, 0);
    disconnect(application, &Application::closing, this, 0);
    disconnect(application, &unityapi::ApplicationInfoInterface::focusRequested, this, 0);

    // don't remove (as it's already being removed) but still delete the guy.
    disconnect(application, &Application::stopped, this, 0);
    connect(application, &Application::stopped, this, [application]() { application->deleteLater(); });

    m_modelUnderChange = false;

    DEBUG_MSG << "(appId=" << application->appId() << ") - after " << toString();
}

void ApplicationManager::move(int from, int to) {
    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::move - from=" << from << "to=" << to;
    if (from == to) return;

    Q_ASSERT(!m_modelUnderChange);
    m_modelUnderChange = true;

    if (from >= 0 && from < m_applications.size() && to >= 0 && to < m_applications.size()) {
        QModelIndex parent;
        /* When moving an item down, the destination index needs to be incremented
           by one, as explained in the documentation:
           http://qt-project.org/doc/qt-5.0/qtcore/qabstractitemmodel.html#beginMoveRows */

        beginMoveRows(parent, from, from, parent, to + (to > from ? 1 : 0));
        m_applications.move(from, to);
        endMoveRows();
    }

    m_modelUnderChange = false;

    qCDebug(QTMIR_APPLICATIONS) << "ApplicationManager::move after " << toString();
}

QModelIndex ApplicationManager::findIndex(Application* application)
{
    for (int i = 0; i < m_applications.size(); ++i) {
        if (m_applications.at(i) == application) {
            return index(i);
        }
    }

    return QModelIndex();
}

QString ApplicationManager::toString() const
{
    QString result;
    for (int i = 0; i < m_applications.count(); ++i) {
        if (i > 0) {
            result.append(",");
        }
        result.append(m_applications.at(i)->appId());
    }
    return result;
}

Application *ApplicationManager::findClosingApplication(const QString &inputAppId) const
{
    const QString appId = toShortAppIdIfPossible(inputAppId);

    for (Application *app : m_closingApplications) {
        if (app->appId() == appId) {
            return app;
        }
    }
    return nullptr;
}

void ApplicationManager::onSessionAboutToCreateSurface(
        const std::shared_ptr<mir::scene::Session> &session, int type, QSize &size)
{
    if (type == mir_surface_type_normal) {
        Application* application = findApplicationWithSession(session);

        if (application) {
            qCDebug(QTMIR_APPLICATIONS).nospace() << "ApplicationManager::onSessionAboutToCreateSurface appId="
                << application->appId();
            size = application->initialSurfaceSize();
        } else {
            qCDebug(QTMIR_APPLICATIONS).nospace() << "ApplicationManager::onSessionAboutToCreateSurface unknown app";
        }
    } else {
        qCDebug(QTMIR_APPLICATIONS).nospace() << "ApplicationManager::onSessionAboutToCreateSurface type=" << type
            << " NOOP";
    }
}

void ApplicationManager::updateFocusedApplication()
{
    Application *focusedApplication = nullptr;
    Application *previouslyFocusedApplication = nullptr;

    auto surface = static_cast<qtmir::MirSurfaceInterface*>(MirFocusController::instance()->focusedSurface());
    if (surface) {
        focusedApplication = findApplication(surface);
    }

    surface = static_cast<qtmir::MirSurfaceInterface*>(MirFocusController::instance()->previouslyFocusedSurface());
    if (surface) {
        previouslyFocusedApplication = findApplication(surface);
    }

    if (focusedApplication != previouslyFocusedApplication) {
        if (focusedApplication) {
            DEBUG_MSG << "() focused " << focusedApplication->appId();
            Q_EMIT focusedApplication->focusedChanged(true);
            this->move(this->m_applications.indexOf(focusedApplication), 0);
        }
        if (previouslyFocusedApplication) {
            DEBUG_MSG << "() unfocused " << previouslyFocusedApplication->appId();
            Q_EMIT previouslyFocusedApplication->focusedChanged(false);
        }
        Q_EMIT focusedApplicationIdChanged();
    }
}

Application *ApplicationManager::findApplication(qtmir::MirSurfaceInterface* surface)
{
    Q_FOREACH (Application *app, m_applications) {
        if (app->session() == surface->session()) {
            return app;
        }
    }
    return nullptr;
}

} // namespace qtmir
