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

void start_xsession()
{
    // When we get a request to stop, we don't quit but rather start xsession
    // in the background.  When xsession finishes loading, we'll be stopped
    // by upstart.

    // But first, stop maliit-server, it needs to be started by unity8
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

    QQuickView view;
    QObject::connect(view.engine(), &QQmlEngine::quit, start_xsession);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.engine()->addImportPath(PLUGIN_PRIVATE_MODULE_DIR);
    view.engine()->addImportPath(PLUGIN_QML_DIR);
    view.setSource(QUrl("qrc:/qml/main.qml"));
    view.showFullScreen();

    return app.exec();
}
