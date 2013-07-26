/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#include "debug.h"
#include "i18n.h"
#include "plugin-manager.h"

#include <QGuiApplication>
#include <QProcessEnvironment>
#include <QQmlContext>
#include <QUrl>
#include <QQuickView>
#include <QtQml>

using namespace SystemSettings;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    /* read environment variables */
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    if (environment.contains(QLatin1String("SS_LOGGING_LEVEL"))) {
        bool isOk;
        int value = environment.value(
            QLatin1String("SS_LOGGING_LEVEL")).toInt(&isOk);
        if (isOk)
            setLoggingLevel(value);
    }

    initTr(I18N_DOMAIN, NULL);
    /* HACK: force the theme until lp #1098578 is fixed */
    QIcon::setThemeName("ubuntu-mobile");

    /* Parse the commandline options to see if we've been given a panel to load,
     * and other options for the panel.
     */
    QString defaultPlugin;
    QVariantMap pluginOptions;
    QStringList arguments = app.arguments();
    for (int i = 1; i < arguments.count(); i++) {
        const QString &argument = arguments.at(i);
        if (!argument.startsWith('-')) {
            defaultPlugin = argument;
        } else if (argument == "--option") {
            QStringList option = arguments.at(++i).split("=");
            // If no value is given, insert an empty string
            pluginOptions.insert(option.at(0), option.value(1, ""));
        }
    }

    QQuickView view;
    QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()),
                     Qt::QueuedConnection);
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterType<SystemSettings::PluginManager>("SystemSettings", 1, 0, "PluginManager");
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.engine()->addImportPath(PLUGIN_PRIVATE_MODULE_DIR);
    view.engine()->addImportPath(PLUGIN_QML_DIR);
    view.rootContext()->setContextProperty("defaultPlugin", defaultPlugin);
    view.rootContext()->setContextProperty("pluginOptions", pluginOptions);
    view.setSource(QUrl("qrc:/qml/MainWindow.qml"));
    view.show();

    return app.exec();
}
