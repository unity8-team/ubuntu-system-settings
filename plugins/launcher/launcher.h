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

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QDesktopWidget>
#include <QObject>

class LauncherPanelPlugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int screens READ screens NOTIFY screensChanged)
public:
    explicit LauncherPanelPlugin(QObject *parent = nullptr);
    ~LauncherPanelPlugin();
    Q_INVOKABLE QRect screenGeometry(int screen = -1) const;
    Q_INVOKABLE int getCurrentScreenNumber() const;
    int screens() const;
Q_SIGNALS:
    void screensChanged(int newCount);
private:
    QDesktopWidget *m_desktopWidget = nullptr;
};
#endif // LAUNCHER_H
