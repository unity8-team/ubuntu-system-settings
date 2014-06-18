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

#include <libintl.h>
#include <qpa/qplatformnativeinterface.h>
#include <ubuntu/application/ui/session.h>
#include <unity-mir/qmirserver.h>
#include <unity-mir/qmirserverapplication.h>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include "PageList.h"

int startShell(int argc, const char** argv, ShellServerConfiguration* server)
{
    QGuiApplication::setApplicationName("System Settings Wizard");
    QGuiApplication *application;

    application = createQMirServerApplication(argc, (char**)argv, server);

    bindtextdomain(I18N_DOMAIN, NULL);
    textdomain(I18N_DOMAIN);

    QQuickView* view = new QQuickView();
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->setTitle("Qml Phone Shell"); // Fake to be the shell

    QPlatformNativeInterface* nativeInterface = QGuiApplication::platformNativeInterface();
    nativeInterface->setProperty("session", U_SYSTEM_SESSION); // receive all input events

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

    QObject::connect(view->engine(), SIGNAL(quit()), application, SLOT(quit()));

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

    setenv("QT_QPA_PLATFORM", "ubuntumirserver", 1);
    QMirServer* mirServer = createQMirServer(argc, argv);
    return runQMirServerWithClient(mirServer, startShell);
}
