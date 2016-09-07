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

#ifndef CLICK_APICLIENT_H
#define CLICK_APICLIENT_H

#include <QObject>
#include <QList>
#include <QString>
#include <QUrl>
#include <QJsonArray>

namespace UpdatePlugin
{
namespace Click
{
// Interface for a Click API client.
class ApiClient : public QObject
{
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~ApiClient() {};
public Q_SLOTS:
    virtual void cancel() = 0;

    /* Request metadata.
     *
     * Requests metadata from a server given an URL and list of packages. A
     * package is a click_package as described in [1].
     * [1] https://wiki.ubuntu.com/AppStore/Interfaces/ApplicationId
     */
    virtual void requestMetadata(const QUrl &url,
                                 const QList<QString> &packages) = 0;

    // Request a token, see lp:1231422.
    virtual void requestToken(const QUrl &url) = 0;
Q_SIGNALS:
    void metadataRequestSucceeded(const QJsonArray &metadata);
    void tokenRequestSucceeded(const QString &token);
    void networkError();
    void serverError();
    void credentialError();
    /* This signal is emitted when the client wants to cancel any active
     * network requests.
     */
    void abortNetworking();
};
} // Click
} // UpdatePlugin

#endif // CLICK_APICLIENT_H
