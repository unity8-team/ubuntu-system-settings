/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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
#ifndef SYSTEM_UPDATE_H
#define SYSTEM_UPDATE_H

#include "updatemodel.h"
#include "network/accessmanager.h"

#include <QDebug>

namespace UpdatePlugin
{
class SystemUpdate : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(UpdateModel* model READ updates CONSTANT)
public:
    static SystemUpdate *instance();
    static void destroyInstance();

    enum class Status
    {
        StatusIdle,
        StatusCheckingClickUpdates,
        StatusCheckingSystemUpdates,
        StatusCheckingAllUpdates,
        StatusBatchMode, // Installing all updates
        StatusBatchModePaused, // Installing all updates paused
        StatusNetworkError,
        StatusServerError
    };

    // We only need one model, views should use UpdateModelFilter.
    UpdateModel* updates();

    /* Qt recommend only one QNetworkAccessManager per application,
    and it is provided here. */
    Network::Manager* nam();

    Q_INVOKABLE bool isCheckRequired();
    Q_INVOKABLE void checkCompleted();

protected:
    explicit SystemUpdate(QObject *parent = 0);
    explicit SystemUpdate(UpdateModel *model, Network::Manager *nam,
                          QObject *parent = 0);
    ~SystemUpdate() {}

private:
    static SystemUpdate *m_instance;
    UpdateModel *m_model;
    Network::Manager *m_nam;
};

} // UpdatePlugin
#endif // SYSTEM_UPDATE_H
