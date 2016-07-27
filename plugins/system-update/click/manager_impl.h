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

#ifndef CLICK_MANAGER_IMPL_H
#define CLICK_MANAGER_IMPL_H

#include "updatemodel.h"

#include "click/manager.h"
#include "click/client.h"
#include "click/manifest.h"
#include "click/sso.h"
#include "click/tokendownloader.h"
#include "click/tokendownloader_factory.h"

#include <QMap>
#include <QJsonArray>
#include <QSharedPointer>

namespace UpdatePlugin
{
namespace Click
{
class ManagerImpl : public Manager
{
    Q_OBJECT
public:
    explicit ManagerImpl(UpdateModel *model, QObject *parent = 0);
    explicit ManagerImpl(Click::Client *client,
                         Click::Manifest *manifest,
                         Click::SSO *sso,
                         Click::TokenDownloaderFactory *downloadFactory,
                         UpdateModel *model,
                         QObject *parent = 0);
    ~ManagerImpl();

    virtual void check() override;
    virtual void retry(const QString &identifier, const uint &revision) override;
    virtual void launch(const QString &identifier, const uint &revision) override;
    virtual void cancel() override;

    virtual bool authenticated() const override;
    virtual bool checkingForUpdates() const override;

private slots:
    void parseMetadata(const QJsonArray &array);
    void handleManifest(const QJsonArray &manifest);
    void handleManifestFailure();
    void handleTokenDownload(QSharedPointer<Update> update);
    void handleTokenDownloadFailure(QSharedPointer<Update> update);
    void handleCredentials(const UbuntuOne::Token &token);
    void handleCredentialsFailed();
    void handleCommunicationErrors();

private:
    void setAuthenticated(const bool authenticated);
    void setCheckingForUpdates(const bool checking);

    void init();
    void initClient();
    void initManifest();
    void initSSO();
    void initTokenDownloader(const Click::TokenDownloader *downloader);

    void requestMetadata();
    QList<QSharedPointer<Update> > parseManifest(const QJsonArray &manifest);

    void completionCheck();

    Click::Client *m_client;
    Click::Manifest *m_manifest;
    Click::SSO *m_sso;
    Click::TokenDownloaderFactory *m_downloadFactory;
    UpdateModel *m_model;

    QMap<QString, QSharedPointer<Update>> m_candidates;
    UbuntuOne::Token m_authToken;
    bool m_authenticated = true;
    bool m_checking = false;
};
} // Click
} // UpdatePlugin

#endif // CLICK_MANAGER_IMPL_H
