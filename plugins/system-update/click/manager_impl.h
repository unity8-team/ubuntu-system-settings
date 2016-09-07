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

#include "click/apiclient.h"
#include "click/manager.h"
#include "click/manifest.h"
#include "click/sso.h"
#include "click/sessiontoken.h"
#include "click/tokendownloader.h"
#include "click/tokendownloader_factory.h"

#include "network/accessmanager.h"

#include <memory>
#include <QMap>
#include <QJsonArray>
#include <QSharedPointer>
#include <QScopedPointer>

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

    // This constructor enables testing.
    explicit ManagerImpl(UpdateModel *model,
                         Network::Manager *nam,
                         ApiClient *client,
                         Manifest *manifest,
                         SSO *sso,
                         TokenDownloaderFactory *tokenDownloadFactory,
                         SessionToken *token,
                         QObject *parent = nullptr);
    ~ManagerImpl();

    virtual void check() override;
    virtual bool launch(const QString &identifier) override;
    virtual void cancel() override;
    virtual void retry(const QString &identifier, const uint &revision) override;

    virtual bool authenticated() const override;
    virtual bool checkingForUpdates() const override;
private Q_SLOTS:
    void parseMetadata(const QJsonArray &array);
    void handleManifest(const QJsonArray &manifest);
    void handleTokenDownload(QSharedPointer<Update> update);
    void handleTokenDownloadFailure(QSharedPointer<Update> update);
    void handleCredentials(SessionToken *token);
    void handleCredentialsAbsence();
    void handleCredentialsFailed();
    void requestMetadata();
    void completionCheck();
    void handleStateChange();
Q_SIGNALS:
    void checkCanceled();
    void stateChanged();
private:
    enum class State { Idle, Manifest, Metadata, Tokens,
                       TokenComplete, Failed, Complete, Canceled };
    void setState(const State &state);
    State state() const;
    void setAuthenticated(const bool authenticated);

    /* Set up connections on a TokenDownloader. */
    void setup(const TokenDownloader *downloader);

    /* Synchronize db updates with a manifest.
     *
     * Any app in database not in the manifest, is removed. If an db update's
     * remote version matches a manifest update's local version, we assume it to be
     * installed. Any Update's local version is replaced by any new local version.
     */
    void synchronize(const QList<QSharedPointer<Update> > &manifestUpdates);

    /* Parse a manifest.
     *
     * Return a list of Updates from a parsed manifest.
     */
    QList<QSharedPointer<Update> > parseManifest(const QJsonArray &manifest);

    UpdateModel *m_model;
    Network::Manager *m_nam;
    ApiClient *m_client;
    Manifest *m_manifest;
    SSO *m_sso;
    std::unique_ptr<TokenDownloaderFactory> m_tokenDownloadFactory;
    QMap<QString, QSharedPointer<Update>> m_candidates;
    std::unique_ptr<SessionToken> m_sessionToken;
    bool m_authenticated = true;
    State m_state = State::Idle;
    QMap<State, QList<State> > m_transitions;
};
} // Click
} // UpdatePlugin

#endif // CLICK_MANAGER_IMPL_H
