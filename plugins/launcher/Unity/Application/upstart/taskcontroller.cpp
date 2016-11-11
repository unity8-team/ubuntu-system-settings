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
 *
 */

#include "applicationinfo.h"
#include "taskcontroller.h"

// qtmir
#include <logging.h>

// Qt
#include <QStandardPaths>

// upstart
extern "C" {
    #include "ubuntu-app-launch.h"
}
#include <ubuntu-app-launch/registry.h>

namespace ual = ubuntu::app_launch;

namespace qtmir
{
namespace upstart
{

struct TaskController::Private
{
    std::shared_ptr<ual::Registry> registry;
    UbuntuAppLaunchAppObserver preStartCallback = nullptr;
    UbuntuAppLaunchAppObserver startedCallback = nullptr;
    UbuntuAppLaunchAppObserver stopCallback = nullptr;
    UbuntuAppLaunchAppObserver focusCallback = nullptr;
    UbuntuAppLaunchAppObserver resumeCallback = nullptr;
    UbuntuAppLaunchAppPausedResumedObserver pausedCallback = nullptr;
    UbuntuAppLaunchAppFailedObserver failureCallback = nullptr;
};

namespace {
/**
 * @brief toShortAppIdIfPossible
 * @param appId - any string that you think is an appId
 * @return if a valid appId was input, a shortened appId is returned, else returns the input string unaltered
 */
QString toShortAppIdIfPossible(const QString &appId) {
    gchar *package, *application;
    if (ubuntu_app_launch_app_id_parse(appId.toLatin1().constData(), &package, &application, nullptr)) {
        // is long appId, so assemble its short appId
        QString shortAppId = QStringLiteral("%1_%2").arg(package, application);
        g_free(package);
        g_free(application);
        return shortAppId;
    } else {
        return appId;
    }
}

std::shared_ptr<ual::Application> createApp(const QString &inputAppId, std::shared_ptr<ual::Registry> registry)
{
    auto appId = ual::AppID::find(inputAppId.toStdString());
    if (appId.empty()) {
        qCDebug(QTMIR_APPLICATIONS) << "ApplicationController::createApp could not find appId" << inputAppId;
        return {};
    }

    try {
        return ual::Application::create(appId, registry);
    } catch (std::runtime_error e) {
        qCDebug(QTMIR_APPLICATIONS) << "ApplicationController::createApp - UAL failed to create app for appId"
                                    << inputAppId << ":" << e.what();
        return {};
    }
}

} // namespace

TaskController::TaskController()
    : qtmir::TaskController(),
      impl(new Private())
{
    impl->registry = std::make_shared<ual::Registry>();

    impl->preStartCallback = [](const gchar * appId, gpointer userData) {
        auto thiz = static_cast<TaskController*>(userData);
        Q_EMIT(thiz->processStarting(toShortAppIdIfPossible(appId)));
    };

    impl->startedCallback = [](const gchar * appId, gpointer userData) {
        auto thiz = static_cast<TaskController*>(userData);
        Q_EMIT(thiz->applicationStarted(toShortAppIdIfPossible(appId)));
    };

    impl->stopCallback = [](const gchar * appId, gpointer userData) {
        auto thiz = static_cast<TaskController*>(userData);
        Q_EMIT(thiz->processStopped(toShortAppIdIfPossible(appId)));
    };

    impl->focusCallback = [](const gchar * appId, gpointer userData) {
        auto thiz = static_cast<TaskController*>(userData);
        Q_EMIT(thiz->focusRequested(toShortAppIdIfPossible(appId)));
    };

    impl->resumeCallback = [](const gchar * appId, gpointer userData) {
        auto thiz = static_cast<TaskController*>(userData);
        Q_EMIT(thiz->resumeRequested(toShortAppIdIfPossible(appId)));
    };

    impl->pausedCallback = [](const gchar * appId, GPid *, gpointer userData) {
        auto thiz = static_cast<TaskController*>(userData);
        Q_EMIT(thiz->processSuspended(toShortAppIdIfPossible(appId)));
    };

    impl->failureCallback = [](const gchar * appId, UbuntuAppLaunchAppFailed failureType, gpointer userData) {
        TaskController::Error error;
        switch(failureType)
        {
        case UBUNTU_APP_LAUNCH_APP_FAILED_CRASH: error = TaskController::Error::APPLICATION_CRASHED;                    break;
        case UBUNTU_APP_LAUNCH_APP_FAILED_START_FAILURE: error = TaskController::Error::APPLICATION_FAILED_TO_START;    break;
        }

        auto thiz = static_cast<TaskController*>(userData);
        Q_EMIT(thiz->processFailed(toShortAppIdIfPossible(appId), error));
    };

    ubuntu_app_launch_observer_add_app_starting(impl->preStartCallback, this);
    ubuntu_app_launch_observer_add_app_started(impl->startedCallback, this);
    ubuntu_app_launch_observer_add_app_stop(impl->stopCallback, this);
    ubuntu_app_launch_observer_add_app_focus(impl->focusCallback, this);
    ubuntu_app_launch_observer_add_app_resume(impl->resumeCallback, this);
    ubuntu_app_launch_observer_add_app_paused(impl->pausedCallback, this);
    ubuntu_app_launch_observer_add_app_failed(impl->failureCallback, this);
}

TaskController::~TaskController()
{
    ubuntu_app_launch_observer_delete_app_starting(impl->preStartCallback, this);
    ubuntu_app_launch_observer_delete_app_started(impl->startedCallback, this);
    ubuntu_app_launch_observer_delete_app_stop(impl->stopCallback, this);
    ubuntu_app_launch_observer_delete_app_focus(impl->focusCallback, this);
    ubuntu_app_launch_observer_delete_app_resume(impl->resumeCallback, this);
    ubuntu_app_launch_observer_delete_app_paused(impl->pausedCallback, this);
    ubuntu_app_launch_observer_delete_app_failed(impl->failureCallback, this);
}

bool TaskController::appIdHasProcessId(const QString& appId, pid_t pid)
{
    auto app = createApp(appId, impl->registry);
    if (!app) {
        return false;
    }

    for (auto &instance: app->instances()) {
        if (instance->hasPid(pid)) {
            return true;
        }
    }

    return false;
}

bool TaskController::stop(const QString& appId)
{
    auto app = createApp(appId, impl->registry);
    if (!app) {
        return false;
    }

    for (auto &instance: app->instances()) {
        instance->stop();
    }

    return true;
}

bool TaskController::start(const QString& appId, const QStringList& arguments)
{
    auto app = createApp(appId, impl->registry);
    if (!app) {
        return false;
    }

    // Convert arguments QStringList into format suitable for ubuntu-app-launch
    std::vector<ual::Application::URL> urls;
    for (auto &arg: arguments) {
        urls.emplace_back(ual::Application::URL::from_raw(arg.toStdString()));
    }

    app->launch(urls);

    return true;
}

bool TaskController::suspend(const QString& appId)
{
    auto app = createApp(appId, impl->registry);
    if (!app) {
        return false;
    }

    for (auto &instance: app->instances()) {
        instance->pause();
    }

    return true;
}

bool TaskController::resume(const QString& appId)
{
    auto app = createApp(appId, impl->registry);
    if (!app) {
        return false;
    }

    for (auto &instance: app->instances()) {
        instance->resume();
    }

    return true;
}

QSharedPointer<qtmir::ApplicationInfo> TaskController::getInfoForApp(const QString &appId) const
{
    auto app = createApp(appId, impl->registry);
    if (!app || !app->info()) {
        return QSharedPointer<qtmir::ApplicationInfo>();
    }

    QString shortAppId = toShortAppIdIfPossible(QString::fromStdString(std::string(app->appId())));
    auto appInfo = new qtmir::upstart::ApplicationInfo(shortAppId, app->info());
    return QSharedPointer<qtmir::ApplicationInfo>(appInfo);
}

} // namespace upstart
} // namespace qtmir
