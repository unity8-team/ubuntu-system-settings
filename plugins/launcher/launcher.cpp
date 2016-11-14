/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#include "launcher.h"

#include <QGuiApplication>
#include <QApplication>

LauncherPanelPlugin::LauncherPanelPlugin(QObject *parent)
    : QObject(parent)
{
    auto app = (QApplication*) QGuiApplication::instance();
    m_desktopWidget = app->desktop();
    connect(m_desktopWidget, SIGNAL(screenCountChanged(int)),
            this, SIGNAL(screensChanged(int)));
}

LauncherPanelPlugin::~LauncherPanelPlugin()
{
    disconnect(m_desktopWidget, SIGNAL(screenCountChanged(int)),
               this, SIGNAL(screensChanged(int)));
}

QRect LauncherPanelPlugin::screenGeometry(int screen) const
{
    return m_desktopWidget->screenGeometry(screen);
}

int LauncherPanelPlugin::getCurrentScreenNumber() const
{
    return m_desktopWidget->screenNumber(m_desktopWidget);
}

int LauncherPanelPlugin::screens() const
{
    return m_desktopWidget->screenCount();
}
