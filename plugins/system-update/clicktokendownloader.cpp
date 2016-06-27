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

#include "clicktokendownloader.h"
#include "helpers.h"

namespace UpdatePlugin
{
ClickTokenDownloader::ClickTokenDownloader(QObject *parent,
                                           Update *update)
    : QObject(parent)
    , m_update(update)
    , m_apiClient(this)
    , m_authToken(UbuntuOne::Token())
{
    init();
}

ClickTokenDownloader::~ClickTokenDownloader()
{
    cancel();
}

void ClickTokenDownloader::init()
{
    connect(&m_apiClient, SIGNAL(success(QNetworkReply*)),
            this, SLOT(handleSuccess(QNetworkReply*)));
    connect(&m_apiClient, SIGNAL(networkError()),
            this, SIGNAL(tokenRequestFailed(m_update)));
    connect(&m_apiClient, SIGNAL(serverError()),
            this, SIGNAL(tokenRequestFailed(m_update)));
    connect(&m_apiClient, SIGNAL(credentialError()),
            this, SIGNAL(tokenRequestFailed(m_update)));
}

void ClickTokenDownloader::setAuthToken(const UbuntuOne::Token &authToken)
{
    m_authToken = authToken;
}

void ClickTokenDownloader::cancel()
{
    m_apiClient.cancel();
}

void ClickTokenDownloader::requestToken()
{
    qWarning() << "requests token on url" << m_update->identifier();
    if (!m_authToken.isValid() && !Helpers::isIgnoringCredentials()) {
        qWarning() << "token invalid";
        Q_EMIT tokenRequestFailed(m_update);
        return;
    }

    QString authHeader = m_authToken.signUrl(
        m_update->downloadUrl(), QStringLiteral("HEAD"), true
    );

    if (authHeader.isEmpty()) {
        // Already logged.
        tokenRequestFailed(m_update);
        return;
    }

    QString signUrl = Helpers::clickTokenUrl(m_update->downloadUrl());
    QUrl query(signUrl);
    query.setQuery(authHeader);
    QNetworkRequest request;
    request.setUrl(query);

    QNetworkReply *reply = m_apiClient.nam()->head(request);
    m_apiClient.initializeReply(reply);
}

void ClickTokenDownloader::handleSuccess(QNetworkReply *reply)
{
    if (reply->hasRawHeader(X_CLICK_TOKEN)) {
        QString header(reply->rawHeader(X_CLICK_TOKEN));
        // This should inform the world that this click update
        // metadata is enough to start a download & install.
        qWarning() << "setting click token to" << header;
        m_update->setToken(header);
        if (!header.isEmpty()) {
            tokenRequestSucceeded(m_update);
        } else {
            tokenRequestFailed(m_update);
        }
    } else {
        Q_EMIT tokenRequestFailed(m_update);
    }
    reply->deleteLater();
}
}// UpdatePlugin
