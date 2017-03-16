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

#ifndef MOCK_LAUNCHER_PANEL_PLUGIN_H
#define MOCK_LAUNCHER_PANEL_PLUGIN_H

#include "launcher.h"

#include <QMap>

class MockLauncherPanelPlugin : public LauncherPanelPlugin
{
    Q_OBJECT
public:
    explicit MockLauncherPanelPlugin(QObject *parent = nullptr)
        : LauncherPanelPlugin(parent) {};
    virtual ~MockLauncherPanelPlugin() {};
    virtual int screens() const override;
public slots:
    virtual QRect screenGeometry(const int &screen = -1) const override;
    virtual int getCurrentScreenNumber() const override;
    void setScreens(const int &screens); // mock only
    void setCurrentScreenNumber(const int &currentScreenNumber); // mock only
    void setScreenGeometry(const int &screen, const int &x, const int &y,
                           const int &width, const int &height); // mock only
private:
    int m_currentScreenNumber = 0;
    int m_screens = 0;
    QMap<int, QRect> m_geometries;

};


#endif // MOCK_LAUNCHER_PANEL_PLUGIN_H
