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

#include "clickupdate.h"
#include "helpers.h"
#include "update.h"

namespace UpdatePlugin
{
ClickUpdate::ClickUpdate(QObject *parent)
    : Update(parent)
    , m_apiClient(this)
    , m_u1Token(UbuntuOne::Token())
{
    connect(this, SIGNAL(tokenChanged()),
            this, SLOT(handleTokenChanged()));
    connect(&m_apiClient, SIGNAL(success(QNetworkReply*)),
            this, SLOT(handleToken(QNetworkReply*)));
}

ClickUpdate::~ClickUpdate()
{
}

void ClickUpdate::handleTokenChanged()
{
    qWarning() << Q_FUNC_INFO << m_token;
    if (!m_token.isEmpty()) {
        clickTokenRequestSucceeded(this);
    }
}

void ClickUpdate::setU1Token(const UbuntuOne::Token &token)
{
    m_u1Token = token;
}

void ClickUpdate::cancel()
{
    m_apiClient.cancel();
}

void ClickUpdate::requestClickToken()
{
    qWarning() << "click meta:" << name() << "requests token...";
    if (!m_u1Token.isValid() && !Helpers::isIgnoringCredentials()) {
        qWarning() << "click meta:" << name() << "token invalid";
        Q_EMIT clickTokenRequestFailed(this);
        return;
    }

    QString authHeader = m_u1Token.signUrl(
        downloadUrl(), QStringLiteral("HEAD"), true
    );

    if (authHeader.isEmpty()) {
        // Already logged.
        clickTokenRequestFailed(this);
        return;
    }

    QString signUrl = Helpers::clickTokenUrl(downloadUrl());
    QUrl query(signUrl);
    query.setQuery(authHeader);
    QNetworkRequest request;
    request.setUrl(query);

    QNetworkReply *reply = m_apiClient.nam()->head(request);
    m_apiClient.initializeReply(reply);
}

void ClickUpdate::handleToken(QNetworkReply *reply)
{
    if (reply->hasRawHeader(X_CLICK_TOKEN)) {
        QString header(reply->rawHeader(X_CLICK_TOKEN));
        // This should inform the world that this click update
        // metadata is enough to start a download & install.
        qWarning() << "setting click token to" << header;
        setToken(header);
    } else {
        Q_EMIT clickTokenRequestFailed(this);
    }
    reply->deleteLater();
}

// void ClickUpdate::tokenRequestSslFailed(const QList<QSslError> &errors)
// {

// }

// void ClickUpdate::tokenRequestFailed(const QNetworkReply::NetworkError &code)
// {

// }

// void ClickUpdate::tokenRequestSucceeded(const QNetworkReply* reply)
// {

// }

// signals:
//     void signedDownloadUrlChanged();
//     void clickTokenChanged();

//     void anonDownloadUrlChanged();
//     void binaryFilesizeChanged();
//     void changelogChanged();
//     void channelChanged();
//     void contentChanged();
//     void departmentChanged();
//     void downloadSha512Changed();
//     void downloadUrlChanged();
//     void iconUrlChanged();
//     void nameChanged();
//     void originChanged();
//     void packageNameChanged();
//     void revisionChanged();
//     void sequenceChanged();
//     void statusChanged();
//     void titleChanged();
//     void remoteVersionChanged();
//     void localVersionChanged();

//     void downloadUrlSignFailure();

}// UpdatePlugin
