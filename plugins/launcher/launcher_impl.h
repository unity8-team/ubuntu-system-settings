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

#ifndef LAUNCHER_IMPL_H
#define LAUNCHER_IMPL_H

#include "launcher.h"

#include <QDesktopWidget>

class LauncherPanelPluginImpl : public LauncherPanelPlugin
{
    Q_OBJECT
public:
    explicit LauncherPanelPluginImpl(QObject *parent = nullptr);
    ~LauncherPanelPluginImpl();
    virtual int screens() const override;
public slots:
    virtual QRect screenGeometry(const int &screen = -1) const override;
    virtual int getCurrentScreenNumber() const override;
private:
    QDesktopWidget *m_desktopWidget = nullptr;
};

#endif // LAUNCHER_IMPL_H
