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

#ifndef CLICK_CLIENT_H
#define CLICK_CLIENT_H

#include <QObject>
#include <QList>
#include <QString>
#include <QUrl>
#include <QJsonArray>

namespace UpdatePlugin
{
namespace Click
{
class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~Client() {};
public Q_SLOTS:
    // Cancel all network activity.
    virtual void cancel() = 0;

    /* Request metadata.
     *
     * Requests metadata from a server given an URL and list of packages. A
     * package is a click_package as described in [1].
     * [1] https://wiki.ubuntu.com/AppStore/Interfaces/ApplicationId
     */
    virtual void requestMetadata(const QUrl &url,
                                 const QList<QString> &packages) = 0;

    // Request a token.
    virtual void requestToken(const QUrl &url) = 0;
Q_SIGNALS:
    // This signal is emitted whenever a metadata request succeeds.
    void metadataRequestSucceeded(const QJsonArray &metadata);

    // This signal is emitted whenever a token request succeeds.
    void tokenRequestSucceeded(const QString &token);

    // This signal is emitted whenever a network error occur.
    void networkError();

    // This signal is emitted whenever a server error occur.
    void serverError();

    // This signal is emitted whenever a credential error occur.
    void credentialError();

    /* This signal is emitted when the client wants to cancel any active
     * network requests.
     */
    void abortNetworking();
};
} // Click
} // UpdatePlugin

#endif // CLICK_CLIENT_H
