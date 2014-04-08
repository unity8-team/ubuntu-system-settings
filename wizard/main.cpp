/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtQuick/QQuickView>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <qpa/qplatformnativeinterface.h>
#include <QLibrary>
#include <QDebug>
#include <libintl.h>
#include <dlfcn.h>
#include <csignal>
#include <unity-mir/qmirserver.h>

#include "PageList.h"

void start_xsession()
{
    // When we get a request to stop, we don't quit but rather start xsession
    // in the background. When xsession finishes loading, we'll be stopped
    // by upstart.

    // But first, stop maliit-server, it needs to be started by unity8.
    // This was an OSK bug in October, need to discover if it is still a
    // problem, especially once we become a system upstart job.
//    if (system("stop maliit-server") != 0) {
//        qDebug() << "there was an error stopping maliit-server";
//    }

    // Now resume starting xsession, which we interrupted with our upstart job
    QString command = "initctl emit xsession";
    command += " SESSION=" + qgetenv("DESKTOP_SESSION");
    command += " SESSIONTYPE=" + qgetenv("SESSIONTYPE");
    command += " &";
    if (system(command.toLatin1().data()) != 0) {
        qDebug() << "unable to start xsession";
        QGuiApplication::quit(); // just quit if we can't start xsession
    }
}

int startShell(int argc, const char** argv, void* server)
{
    QGuiApplication::setApplicationName("System Settings Wizard");
    QGuiApplication *application;

    QLibrary unityMir("unity-mir", 1);
    unityMir.load();

    typedef QGuiApplication* (*createServerApplication_t)(int&, const char **, void*);
    createServerApplication_t createQMirServerApplication = (createServerApplication_t) unityMir.resolve("createQMirServerApplication");
    if (!createQMirServerApplication) {
        qDebug() << "unable to resolve symbol: createQMirServerApplication";
        return 4;
    }

    application = createQMirServerApplication(argc, argv, server);

    bindtextdomain(I18N_DOMAIN, NULL);
    textdomain(I18N_DOMAIN);

    QQuickView* view = new QQuickView();
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->setTitle("Qml Phone Shell"); // Fake to be the shell

    QPlatformNativeInterface* nativeInterface = QGuiApplication::platformNativeInterface();
    /* Shell is declared as a system session so that it always receives all
       input events.
       FIXME: use the enum value corresponding to SYSTEM_SESSION_TYPE (= 1)
       when it becomes available.
    */
    nativeInterface->setProperty("ubuntuSessionType", 1);
//    view->setProperty("role", 2); // INDICATOR_ACTOR_ROLE

    QString rootDir = qgetenv("UBUNTU_SYSTEM_SETTINGS_WIZARD_ROOT"); // for testing
    if (rootDir.isEmpty())
        rootDir = WIZARD_ROOT;

    PageList pageList;

    QStringList importPaths = view->engine()->importPathList();
    importPaths.replaceInStrings(QRegExp("qt5/imports$"), "qt5/imports/Unity-Mir");
    view->engine()->setImportPathList(importPaths);
    view->engine()->addImportPath(PLUGIN_PRIVATE_MODULE_DIR);
    view->engine()->addImportPath(PLUGIN_QML_DIR);
    view->engine()->addImportPath(SHELL_PLUGINDIR);
    view->rootContext()->setContextProperty("pageList", &pageList);
    view->setSource(QUrl(rootDir + "/qml/main.qml"));
    view->setColor("transparent");

    QObject::connect(view->engine(), &QQmlEngine::quit, start_xsession);

    view->showFullScreen();

    int result = application->exec();

    delete view;
    delete application;

    return result;
}

int main(int argc, const char *argv[])
{
    /* Workaround Qt platform integration plugin not advertising itself
       as having the following capabilities:
        - QPlatformIntegration::ThreadedOpenGL
        - QPlatformIntegration::BufferQueueingOpenGL
    */
    setenv("QML_FORCE_THREADED_RENDERER", "1", 1);
    setenv("QML_FIXED_ANIMATION_STEP", "1", 1);

    // For ubuntumirserver/ubuntumirclient
    setenv("QT_QPA_PLATFORM", "ubuntumirserver", 1);

    // If we use unity-mir directly, we automatically link to the Mir-server
    // platform-api bindings, which result in unexpected behaviour when
    // running the non-Mir scenario.
    QLibrary unityMir("unity-mir", 1);
    unityMir.load();
    if (!unityMir.isLoaded()) {
        qDebug() << "Library unity-mir not found/loaded";
        return 1;
    }

    typedef QMirServer* (*createServer_t)(int, const char **);
    createServer_t createQMirServer = (createServer_t) unityMir.resolve("createQMirServer");
    if (!createQMirServer) {
        qDebug() << "unable to resolve symbol: createQMirServer";
        return 2;
    }

    QMirServer* mirServer = createQMirServer(argc, argv);

    typedef int (*runWithClient_t)(QMirServer*, std::function<int(int, const char**, void*)>);
    runWithClient_t runWithClient = (runWithClient_t) unityMir.resolve("runQMirServerWithClient");
    if (!runWithClient) {
        qDebug() << "unable to resolve symbol: runWithClient";
        return 3;
    }

    return runWithClient(mirServer, startShell);
}
