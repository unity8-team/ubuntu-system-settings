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
#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#include "updatemodel.h"
#include "click/manager.h"
#include "image/imagemanager.h"
#include "network/accessmanager.h"

#include <QDebug>

namespace UpdatePlugin
{
class UpdateManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status Check)
    Q_PROPERTY(UpdateModel* model READ updates CONSTANT)
    Q_PROPERTY(UpdateModelFilter* pendingUpdates READ pendingUpdates CONSTANT)
    Q_PROPERTY(UpdateModelFilter* clickUpdates READ clickUpdates CONSTANT)
    Q_PROPERTY(UpdateModelFilter* imageUpdates READ imageUpdates CONSTANT)
    Q_PROPERTY(UpdateModelFilter* installedUpdates
               READ installedUpdates CONSTANT)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool authenticated READ authenticated
               NOTIFY authenticatedChanged)
public:
    explicit UpdateManager(QObject *parent = nullptr);
    // This constructor enables testing.
    explicit UpdateManager(UpdateModel *model,
                          Network::Manager *nam,
                          Image::Manager *imageManager,
                          Click::Manager *clickManager,
                          QObject *parent = nullptr);
    ~UpdateManager() {};
    enum class Status : uint
    {
        StatusIdle,
        StatusCheckingClickUpdates,
        StatusCheckingImageUpdates,
        StatusCheckingAllUpdates,
        StatusNetworkError,
        StatusServerError
    };

    /* Representing the user's intention when performing a check. */
    enum class Check : uint
    {
        CheckIfNecessary,
        CheckAll,
        CheckClick,
        CheckImage
    };

    UpdateModel* updates();
    UpdateModelFilter* pendingUpdates();
    UpdateModelFilter* clickUpdates();
    UpdateModelFilter* imageUpdates();
    UpdateModelFilter* installedUpdates();
    Status status() const;

    Q_INVOKABLE void check(const Check check = Check::CheckIfNecessary);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE bool launch(const QString &identifier);
    Q_INVOKABLE void retry(const QString &identifier, const uint &revision);
    Q_INVOKABLE void remove(const QString &identifier, const uint &revision);

    /* Return whether or not a check is required.
     *
     * The specifications says that a manual check should not happen if a
     * check was completed less than 30 minutes ago, so 30 minutes [1] must
     * have passed since the last check for this return to be true.
     *
     * [1]https://wiki.ubuntu.com/SoftwareUpdates#Checking_for_updates_manually
     */
    bool isCheckRequired();

    // Return whether or not the user is authenticated.
    bool authenticated();
protected:
    UpdateModel *m_model;
    Status m_status = Status::StatusIdle;
Q_SIGNALS:
    void statusChanged();
    void authenticatedChanged();
private Q_SLOTS:
    void calculateStatus();
    void handleCheckCompleted();
    void handleNetworkError();
    void handleServerError();
private:
    void setStatus(const Status &status);
    void init();

    Network::Manager *m_nam;
    UpdateModelFilter m_pending;
    UpdateModelFilter m_clicks;
    UpdateModelFilter m_images;
    UpdateModelFilter m_installed;
    Image::Manager *m_imageManager;
    Click::Manager *m_clickManager;
};
} // UpdatePlugin
#endif // UPDATE_MANAGER_H
