/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
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

#include <libintl.h>
#include <QGuiApplication>
#include <QQmlContext>
#include <QUrl>
#include <QQuickView>
#include <QtQml>

#include "PageList.h"

void start_xsession()
{
    // When we get a request to stop, we don't quit but rather start xsession
    // in the background.  When xsession finishes loading, we'll be stopped
    // by upstart.

    // But first, stop maliit-server, it needs to be started by unity8.
    // This was an OSK bug in October, need to discover if it is still a
    // problem, especially once we become a system upstart job.
    if (system("stop maliit-server") != 0)
    {} // ignore any errors

    // Now resume starting xsession, which we interrupted with our upstart job
    QString command = "initctl emit xsession";
    command += " SESSION=" + qgetenv("DESKTOP_SESSION");
    command += " SESSIONTYPE=" + qgetenv("SESSIONTYPE");
    command += " &";
    if (system(command.toLatin1().data()) != 0)
        QGuiApplication::quit(); // just quit if we can't start xsession
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    bindtextdomain(I18N_DOMAIN, NULL);
    textdomain(I18N_DOMAIN);

    QString rootDir = qgetenv("UBUNTU_SYSTEM_SETTINGS_WIZARD_ROOT"); // for testing
    if (rootDir.isEmpty())
        rootDir = WIZARD_ROOT;

    PageList pageList;
    QQuickView view;
    QObject::connect(view.engine(), &QQmlEngine::quit, start_xsession);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.engine()->addImportPath(PLUGIN_PRIVATE_MODULE_DIR);
    view.engine()->addImportPath(PLUGIN_QML_DIR);
    view.engine()->addImportPath(SHELL_PLUGINDIR);
    view.rootContext()->setContextProperty("pageList", &pageList);
    view.setSource(QUrl(rootDir + "/qml/main.qml"));
//    view.show();
    view.showFullScreen();

    return app.exec();
}


// Qt
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

// local
#include <paths.h>
#include "MouseTouchAdaptor.h"
#include "ApplicationArguments.h"

#include <unity-mir/qmirserver.h>


int startShell(int argc, const char** argv, void* server)
{
    QGuiApplication::setApplicationName("Unity 8");
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

    bindtextdomain("unity8", translationDirectory().toUtf8().data());

    QQuickView* view = new QQuickView();
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->setTitle("Qml Phone Shell");
    view->engine()->setBaseUrl(QUrl::fromLocalFile(::qmlDirectory()));
    if (args.contains(QLatin1String("-frameless"))) {
        view->setFlags(Qt::FramelessWindowHint);
    }

    QPlatformNativeInterface* nativeInterface = QGuiApplication::platformNativeInterface();
    /* Shell is declared as a system session so that it always receives all
       input events.
       FIXME: use the enum value corresponding to SYSTEM_SESSION_TYPE (= 1)
       when it becomes available.
    */
    nativeInterface->setProperty("ubuntuSessionType", 1);
    view->setProperty("role", 2); // INDICATOR_ACTOR_ROLE

    QUrl source(::qmlDirectory()+"Shell.qml");
    prependImportPaths(view->engine(), ::overrideImportPaths());
    appendImportPaths(view->engine(), ::fallbackImportPaths());

    QStringList importPaths = view->engine()->importPathList();
    importPaths.replaceInStrings(QRegExp("qt5/imports$"), "qt5/imports/Unity-Mir");
    view->engine()->setImportPathList(importPaths);

    view->setSource(source);
    view->setColor("transparent");

    view->showFullScreen();

    int result = application->exec();

    delete view;
    delete mouseTouchAdaptor;
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
