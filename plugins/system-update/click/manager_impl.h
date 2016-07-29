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

#include "network/accessmanager.h"

#include <QMap>
#include <QJsonArray>
#include <QSharedPointer>
#include <QStateMachine>

namespace UpdatePlugin
{
namespace Click
{
class ManagerImpl : public Manager
{
    Q_OBJECT
public:
    explicit ManagerImpl(UpdateModel *model, Network::Manager *nam,
                         QObject *parent = nullptr);
    explicit ManagerImpl(UpdateModel *model,
                         Network::Manager *nam,
                         Click::Client *client,
                         Click::Manifest *manifest,
                         Click::SSO *sso,
                         Click::TokenDownloaderFactory *tokenDownloadFactory,
                         QObject *parent = nullptr);
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
    void handleTokenDownload(QSharedPointer<Update> update);
    void handleTokenDownloadFailure(QSharedPointer<Update> update);
    void handleCredentials(const UbuntuOne::Token &token);
    void handleCredentialsFailed();
    void requestMetadata();
    void completionCheck();
    void handleStateChange();
Q_SIGNALS:
    void checkCanceled();
    void stateChanged();
private:
    enum class State { Idle, Manifest, Metadata, Tokens, TokenComplete, Failed,
                       Complete, Canceled };
    void setState(const State &state);
    State state() const;
    void setAuthenticated(const bool authenticated);
    void initTokenDownloader(const Click::TokenDownloader *downloader);
    QList<QSharedPointer<Update> > parseManifest(const QJsonArray &manifest);

    UpdateModel *m_model;
    Network::Manager *m_nam;
    Click::Client *m_client;
    Click::Manifest *m_manifest;
    Click::SSO *m_sso;
    Click::TokenDownloaderFactory *m_tokenDownloadFactory;

    QMap<QString, QSharedPointer<Update>> m_candidates;
    UbuntuOne::Token m_authToken;
    bool m_authenticated = true;
    State m_state = State::Idle;
    QMap<State, QList<State> > m_transitions;
};
} // Click
} // UpdatePlugin

#endif // CLICK_MANAGER_IMPL_H
