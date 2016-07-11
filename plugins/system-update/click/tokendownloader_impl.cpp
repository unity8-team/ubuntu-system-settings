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

#include "helpers.h"
#include "tokendownloader_impl.h"

namespace UpdatePlugin
{
namespace Click
{
TokenDownloaderImpl::TokenDownloaderImpl(Client *client,
                                         QSharedPointer<Update> update,
                                         QObject *parent)
    : TokenDownloader(update, parent)
    , m_client(client)
{
    init();
}

TokenDownloaderImpl::~TokenDownloaderImpl()
{
    cancel();
}

void TokenDownloaderImpl::init()
{
    connect(m_client, SIGNAL(tokenRequestSucceeded(const QString)),
            this, SLOT(handleSuccess(const QString)));
    connect(m_client, SIGNAL(networkError()),
            this, SLOT(handleFailure()));
    connect(m_client, SIGNAL(serverError()),
            this, SLOT(handleFailure()));
    connect(m_client, SIGNAL(credentialError()),
            this, SLOT(handleFailure()));
}

void TokenDownloaderImpl::setAuthToken(const UbuntuOne::Token &authToken)
{
    m_authToken = authToken;
}

void TokenDownloaderImpl::cancel()
{
    m_client->cancel();
}

void TokenDownloaderImpl::download()
{
    if (!m_authToken.isValid() && !Helpers::isIgnoringCredentials()) {
        Q_EMIT downloadFailed(m_update);
        return;
    }

    QString authHeader;
    if (!Helpers::isIgnoringCredentials()) {
        authHeader = m_authToken.signUrl(
            m_update->downloadUrl(), QStringLiteral("HEAD"), true
        );

        if (authHeader.isEmpty()) {
            // Already logged.
            downloadFailed(m_update);
            return;
        }
    }

    QString signUrl = Helpers::clickTokenUrl(m_update->downloadUrl());
    QUrl query(signUrl);
    query.setQuery(authHeader);
    m_client->requestToken(query);
}

void TokenDownloaderImpl::handleSuccess(const QString &token)
{
    m_update->setToken(token);
    if (token.isEmpty()) {
        downloadFailed(m_update);
    } else {
        downloadSucceeded(m_update);
    }
}

void TokenDownloaderImpl::handleFailure()
{
    Q_EMIT downloadFailed(m_update);
}
}// Click
}// UpdatePlugin