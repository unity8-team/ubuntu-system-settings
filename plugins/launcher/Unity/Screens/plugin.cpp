/*
 * Copyright (C) 2016 Canonical, Ltd.
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
#include <QQmlExtensionPlugin>
#include <QtQml/qqml.h>
#include <QScreen>

// local
#include "screens.h"
#include "qquickscreenwindow.h"

using namespace qtmir;

class UnityScreensPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")

    virtual void registerTypes(const char* uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Unity.Screens"));

        qRegisterMetaType<QScreen*>("QScreen*");

        qmlRegisterType<qtmir::Screens>(uri, 0, 1, "Screens");
        qRegisterMetaType<qtmir::Screens::FormFactor>("Screens::FormFactor");

        qmlRegisterType<qtmir::QQuickScreenWindow>(uri, 0, 1, "ScreenWindow");
    }
};

#include "plugin.moc"
