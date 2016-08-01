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

#ifndef FAKE_TOKEN_DOWNLOADER_H
#define FAKE_TOKEN_DOWNLOADER_H

#include "click/sessiontoken.h"
#include "click/tokendownloader.h"

using namespace UpdatePlugin;

class MockTokenDownloader : public Click::TokenDownloader
{
public:
    explicit MockTokenDownloader(Click::ApiClient *client,
                                 QSharedPointer<Update> update,
                                 QObject *parent = nullptr)
        : Click::TokenDownloader(client, update, parent)
    {
        downloadUrl = update->downloadUrl();
    }

    virtual void download(const QString &url) override
    {
        Q_UNUSED(url)
    }

    virtual Click::ApiClient* client() const override
    {
        return m_client;
    }

    void mockDownloadSucceeded(const QString &token)
    {
        m_update->setToken(token);
        Q_EMIT downloadSucceeded(m_update);
    }

    void mockDownloadFailed()
    {
        Q_EMIT downloadFailed(m_update);
    }

    void mockCredentialError()
    {
        Q_EMIT credentialError();
    }

    QString downloadUrl;
public slots:
    virtual void cancel() override {}
};

#endif // FAKE_TOKEN_DOWNLOADER_H
