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
    Q_PROPERTY(UpdateModelFilter* pendingUpdates READ pendingUpdates CONSTANT)
    Q_PROPERTY(UpdateModelFilter* clickUpdates READ clickUpdates CONSTANT)
    Q_PROPERTY(UpdateModelFilter* imageUpdates READ imageUpdates CONSTANT)
    Q_PROPERTY(UpdateModelFilter* installedUpdates READ installedUpdates CONSTANT)
public:
    static SystemUpdate *instance();
    static void destroyInstance();

    enum class Status
    {
        StatusIdle,
        StatusCheckingClickUpdates,
        StatusCheckingSystemUpdates,
        StatusCheckingAllUpdates,
        StatusNetworkError,
        StatusServerError
    };

    UpdateModel* updates();
    UpdateModelFilter* pendingUpdates() const;
    UpdateModelFilter* clickUpdates() const;
    UpdateModelFilter* imageUpdates() const;
    UpdateModelFilter* installedUpdates() const;

    /* Qt recommend only one QNetworkAccessManager per application,
    and it is provided here. */
    Network::Manager* nam();

    Q_INVOKABLE bool isCheckRequired();
    Q_INVOKABLE void checkCompleted();

protected:
    explicit SystemUpdate(QObject *parent = 0);
    explicit SystemUpdate(UpdateModel *model,
                          UpdateModelFilter *pending,
                          UpdateModelFilter *clicks,
                          UpdateModelFilter *images,
                          UpdateModelFilter *installed,
                          Network::Manager *nam,
                          QObject *parent = 0);
    ~SystemUpdate() {}

private:
    void init();
    static SystemUpdate *m_instance;
    UpdateModel *m_model;
    UpdateModelFilter *m_pending;
    UpdateModelFilter *m_clicks;
    UpdateModelFilter *m_images;
    UpdateModelFilter *m_installed;
    Network::Manager *m_nam;
};

} // UpdatePlugin
#endif // SYSTEM_UPDATE_H
