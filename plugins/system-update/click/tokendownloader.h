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

#ifndef CLICK_TOKEN_DOWNLOADER_H
#define CLICK_TOKEN_DOWNLOADER_H

#include "update.h"
#include "click/apiclient.h"

#include <QObject>
#include <QSharedPointer>

namespace UpdatePlugin
{
namespace Click
{
/* Performs the dance described in [1].
 *
 * This dance is only performed at check time, not for retries and other
 * scenarios in which a click download is expected to happen within 5 minutes.
 * The token this downloader downloads is valid for 24 hours.
 *
 * [1] lp:1231422
 */
class TokenDownloader : public QObject
{
    Q_OBJECT
public:
    explicit TokenDownloader(ApiClient *client,
                             QSharedPointer<Update> update,
                             QObject *parent = nullptr)
        : QObject(parent)
        , m_client(client)
        , m_update(update)
    {
    }
    virtual ~TokenDownloader() {};

    /* Downloads a token.
     *
     * URL should be a oauth signed URL, to be used to perform a HEAD request.
     * For details, see lp:1231422.
     */
    virtual void download(const QString &url) = 0;
    virtual ApiClient* client() const = 0;
public Q_SLOTS:
    virtual void cancel() = 0;
Q_SIGNALS:
    void downloadSucceeded(QSharedPointer<Update> update);
    void downloadFailed(QSharedPointer<Update> update);
    void credentialError();
protected:
    ApiClient *m_client;
    QSharedPointer<Update> m_update;
};
} // Click
} // UpdatePlugin

#endif // CLICK_TOKEN_DOWNLOADER_H
