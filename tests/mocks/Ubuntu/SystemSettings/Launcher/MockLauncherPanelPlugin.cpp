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

#include "MockLauncherPanelPlugin.h"

#include <QDebug>

QRect MockLauncherPanelPlugin::screenGeometry(const int &screen) const
{
    return m_geometries.value(screen);
}

int MockLauncherPanelPlugin::getCurrentScreenNumber() const
{
    return m_currentScreenNumber;
}

int MockLauncherPanelPlugin::screens() const
{
    return m_screens;
}

void MockLauncherPanelPlugin::setScreens(const int &screens)
{
    m_screens = screens;
    Q_EMIT screensChanged(m_screens);
}

void MockLauncherPanelPlugin::setCurrentScreenNumber(const int &currentScreenNumber)
{
    m_currentScreenNumber = currentScreenNumber;
}

void MockLauncherPanelPlugin::setScreenGeometry(
    const int &screen, const int &x, const int &y, const int &width,
    const int &height)

{
    QRect rect(x, y, width, height);
    m_geometries.insert(screen, rect);
    Q_EMIT screensChanged(m_screens);
}
