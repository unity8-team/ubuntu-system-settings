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
#ifndef MOCK_UPDATE_H
#define MOCK_UPDATE_H

#include <QObject>

class MockUpdate : public QObject
{
    Q_OBJECT
    Q_ENUMS(Kind)
    Q_ENUMS(State)
public:
    MockUpdate(QObject *parent = 0) : QObject(parent) {};
    ~MockUpdate() {};

    enum class Kind : uint
    {
        KindUnknown = 1,
        KindClick = 2,
        KindImage = 4
    };

    enum class State : uint
    {
        StateUnknown = 0,
        StateAvailable,
        StateUnavailable,
        StateQueuedForDownload,
        StateDownloading,
        StateDownloadingAutomatically,
        StateDownloadPaused,
        StateAutomaticDownloadPaused,
        StateInstalling,
        StateInstallingAutomatically,
        StateInstallPaused,
        StateInstallFinished,
        StateInstalled,
        StateDownloaded,
        StateFailed
    };
};

#endif // MOCK_UPDATE_H
