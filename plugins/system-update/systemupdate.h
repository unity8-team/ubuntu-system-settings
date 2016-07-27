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
#include "click/manager.h"
#include "image/imagemanager.h"
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
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool authenticated READ authenticated NOTIFY authenticatedChanged)
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
    Status status() const;

    /* Qt recommend only one QNetworkAccessManager per application,
    and it is provided here. */
    Network::Manager* nam();

    Q_INVOKABLE void check();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void retry(const QString &identifier, const uint &revision);
    Q_INVOKABLE void launch(const QString &identifier, const uint &revision);
    bool isCheckRequired();
    bool authenticated();

Q_SIGNALS:
    void statusChanged();
    void authenticatedChanged();

protected:
    explicit SystemUpdate(QObject *parent = 0);
    explicit SystemUpdate(UpdateModel *model,
                          UpdateModelFilter *pending,
                          UpdateModelFilter *clicks,
                          UpdateModelFilter *images,
                          UpdateModelFilter *installed,
                          Network::Manager *nam,
                          Image::Manager *imageManager,
                          Click::Manager *clickManager,
                          QObject *parent = 0);
    ~SystemUpdate() {}

private slots:
    void calculateStatus();
    void handleCheckCompleted();
    void handleNetworkError();
    void handleServerError();

private:
    void init();
    void setStatus(const Status &status);

    static SystemUpdate *m_instance;
    UpdateModel *m_model;
    UpdateModelFilter *m_pending;
    UpdateModelFilter *m_clicks;
    UpdateModelFilter *m_images;
    UpdateModelFilter *m_installed;
    Network::Manager *m_nam;
    Image::Manager *m_imageManager;
    Click::Manager *m_clickManager;

    Status m_status = Status::StatusIdle;
};

} // UpdatePlugin
#endif // SYSTEM_UPDATE_H
