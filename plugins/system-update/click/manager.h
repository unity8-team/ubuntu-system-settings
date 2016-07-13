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

#include "systemupdate.h"
#include "updatemodel.h"

#include "click/client.h"
#include "click/manifest.h"
#include "click/sso.h"
#include "click/tokendownloader.h"
#include "click/tokendownloader_factory.h"

#include <QHash>
#include <QProcess>
#include <QByteArray>
#include <QJsonArray>
#include <QSharedPointer>

namespace UpdatePlugin
{
namespace Click
{
class Manager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool authenticated READ authenticated
               NOTIFY authenticatedChanged)
public:
    explicit Manager(QObject *parent = 0);
    explicit Manager(Click::Client *client,
                                Click::Manifest *manifest,
                                Click::SSO *sso,
                                Click::TokenDownloaderFactory *downloadFactory,
                                UpdateModel *model,
                                QObject *parent = 0);
    ~Manager();

    Q_INVOKABLE void check();
    Q_INVOKABLE void retry(const QString &identifier, const uint &revision);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void launch(const QString &appId);

    bool authenticated();

private slots:
    void handleManifestSuccess(const QJsonArray &manifest);
    void handleManifestFailure();
    void handleMetadataSuccess(const QByteArray &metadata);
    void handleTokenDownload(QSharedPointer<Update> update);
    void handleTokenDownloadFailure(QSharedPointer<Update> update);
    void handleCredentialsFound(const UbuntuOne::Token &token);
    void handleCredentialsFailed();
    void handleCommunicationErrors();
    void handleCheckStart() { m_checking = true; }
    void handleCheckStop() { m_checking = false; }

signals:
    void authenticatedChanged();

    void checkStarted();
    void checkCompleted();
    void checkCanceled();
    void checkFailed();

    void networkError();
    void serverError();
    void credentialError();

private:
    void setAuthenticated(const bool authenticated);

    void init();
    void initClient();
    void initManifest();
    void initSSO();
    void initTokenDownloader(const Click::TokenDownloader *downloader);

    void requestMetadata();
    void parseMetadata(const QJsonArray &array);

    void completionCheck();

    Click::Client *m_client;
    Click::Manifest *m_manifest;
    Click::SSO *m_sso;
    Click::TokenDownloaderFactory *m_downloadFactory;
    UpdateModel *m_model;

    QHash<QString, QSharedPointer<Update>> m_updates;
    UbuntuOne::Token m_authToken;
    bool m_authenticated = true;
    bool m_checking = false;
};
} // Click
} // UpdatePlugin

#endif // CLICK_MANAGER_H
