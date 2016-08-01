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
    explicit Manager(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~Manager() {}

    // Check for updates.
    virtual void check() = 0;

    /* Cancel all activity.
     *
     * Will cancel activity in any managed objects (TokenDownloaders).
     */
    virtual void cancel() = 0;

    /* Launch app.
     *
     * Tries to launch an app, but will only succeed if a APP_ID can be created
     */
    virtual void launch(const QString &identifier, const uint &revision) = 0;

    /* Retry app.
     *
     * Currently, this merely updates the signedDownloadUrl on an Update.
     */
    virtual void retry(const QString &identifier, const uint &revision) = 0;

    // Return whether or not a user is authenticated.
    virtual bool authenticated() const = 0;

    // Return whether or not this manager is currently checking for updates.
    virtual bool checkingForUpdates() const = 0;
Q_SIGNALS:
    /* This signal is emitted when the user is either authenticated or
     * de-authenticated.
     */
    void authenticatedChanged();

    // This signal is emitted when check status changes.
    void checkingForUpdatesChanged();

    // This signal is emitted when a check completes.
    void checkCompleted();

    // This signal is emitted whenever a network error occur.
    void networkError();

    // This signal is emitted whenever a server error occur.
    void serverError();

    // This signal is emitted whenever a credential error occur.
    void credentialError();
};
} // Click
} // UpdatePlugin

#endif // CLICK_MANAGER_H
