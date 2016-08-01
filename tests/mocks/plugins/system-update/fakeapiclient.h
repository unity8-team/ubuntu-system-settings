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

#ifndef FAKE_CLICK_CLIENT_H
#define FAKE_CLICK_CLIENT_H

#include "click/apiclient.h"

namespace UpdatePlugin
{
class MockApiClient : public Click::ApiClient
{
public:
    explicit MockApiClient(QObject* parent = nullptr)
        : Click::ApiClient(parent) {}

    virtual void cancel() override
    {
    }

    virtual void requestMetadata(const QUrl &url,
                                 const QList<QString> &packages) override
    {
        requestedUrl = url;
        requestedPackages = packages;
    }

    virtual void requestToken(const QUrl &url) override
    {
        requestedUrl = url;
    }

    void mockMetadataRequestSucceeded(const QJsonArray &metadata)
    {
        Q_EMIT metadataRequestSucceeded(metadata);
    }

    void mockTokenRequestSucceeded(const QString &token)
    {
        Q_EMIT tokenRequestSucceeded(token);
    }

    void mockNetworkError()
    {
        Q_EMIT networkError();
    }

    void mockServerError()
    {
        Q_EMIT serverError();
    }

    void mockCredentialError()
    {
        Q_EMIT credentialError();
    }

    QUrl requestedUrl;
    QList<QString> requestedPackages;
};
} // UpdatePlugin

#endif // FAKE_CLICK_CLIENT_H
