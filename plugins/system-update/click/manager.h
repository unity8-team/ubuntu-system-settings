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

#ifndef CLICK_MANAGER_H
#define CLICK_MANAGER_H

#include <QObject>
#include <QString>
#include <QDebug>

namespace UpdatePlugin
{
namespace Click
{
/* Interface for a Click package manager. Note that this interface does not
 * handle actual downloads and installations. See DownloadHandler, which is
 * the component that communicates with Ubuntu Download Manager.
 */
class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~Manager() {};
    virtual void check() = 0;
    virtual void cancel() = 0;

    /* Launch app.
     *
     * Tries to launch an app, but will only succeed if a APP_ID can be created
     */
    virtual bool launch(const QString &identifier) = 0;

    /* Retry app.
     *
     * Currently, this merely updates the signedDownloadUrl on an Update.
     */
    virtual void retry(const QString &identifier, const uint &revision) = 0;
    virtual bool authenticated() const = 0;
    virtual bool checkingForUpdates() const = 0;
Q_SIGNALS:
    void authenticatedChanged();
    void checkingForUpdatesChanged();
    void checkCompleted();
    void networkError();
    void serverError();
    void credentialError();
};
} // Click
} // UpdatePlugin

#endif // CLICK_MANAGER_H
