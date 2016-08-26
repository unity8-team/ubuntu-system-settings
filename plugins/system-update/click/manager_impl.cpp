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
#include "click/apiclient_impl.h"
#include "click/manager_impl.h"
#include "click/manifest_impl.h"
#include "click/sessiontoken_impl.h"
#include "click/sso_impl.h"
#include "click/tokendownloader_factory_impl.h"
#include "network/accessmanager_impl.h"
#include "../../src/i18n.h"

#include <ubuntu-app-launch.h>

#include <QDateTime>
#include <QFinalState>
#include <QState>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>

namespace UpdatePlugin
{
namespace Click
{
ManagerImpl::ManagerImpl(UpdateModel *model, Network::Manager *nam,
                         QObject *parent)
    : ManagerImpl(model,
                  nam,
                  new ApiClientImpl(nam),
                  new ManifestImpl,
                  new SSOImpl,
                  new TokenDownloaderFactoryImpl,
                  new SessionTokenImpl,
                  parent)
{
    m_client->setParent(this);
    m_manifest->setParent(this);
    m_sso->setParent(this);
}

ManagerImpl::ManagerImpl(UpdateModel *model,
                         Network::Manager *nam,
                         ApiClient *client,
                         Manifest *manifest,
                         SSO *sso,
                         TokenDownloaderFactory *tokenDownloadFactory,
                         SessionToken *token,
                         QObject *parent)
    : Manager(parent)
    , m_model(model)
    , m_nam(nam)
    , m_client(client)
    , m_manifest(manifest)
    , m_sso(sso)
    , m_tokenDownloadFactory(tokenDownloadFactory)
    , m_sessionToken(token)
{
    /* Request a manifest.
     *
     * This will produce a manifest with which we will synchronize our
     * database.
     */
    m_manifest->request();

    connect(this, SIGNAL(stateChanged()), this, SLOT(handleStateChange()));
    connect(this, SIGNAL(stateChanged()), this, SIGNAL(checkingForUpdatesChanged()));
    connect(m_client, SIGNAL(metadataRequestSucceeded(const QJsonArray&)),
            this, SLOT(parseMetadata(const QJsonArray&)));
    connect(m_client, SIGNAL(networkError()), this, SIGNAL(networkError()));
    connect(m_client, SIGNAL(serverError()), this, SIGNAL(serverError()));
    connect(m_client, SIGNAL(credentialError()),
            this, SIGNAL(credentialError()));
    connect(m_client, &ApiClient::serverError, this, [this]() {
        setState(State::Failed);
    });
    connect(m_client, &ApiClient::networkError, this, [this]() {
        setState(State::Failed);
    });
    connect(m_client, SIGNAL(credentialError()),
            this, SLOT(handleCredentialsFailed()));
    connect(this, SIGNAL(checkCanceled()), m_client, SLOT(cancel()));

    connect(m_manifest, SIGNAL(requestSucceeded(const QJsonArray&)),
            this, SLOT(handleManifest(const QJsonArray&)));
    connect(m_manifest, &Manifest::requestFailed, this, [this]() {
        setState(State::Complete);
    });
    connect(m_sso, SIGNAL(credentialsFound(SessionToken*)),
            this, SLOT(handleCredentials(SessionToken*)));
    connect(m_sso, SIGNAL(credentialsNotFound()),
            this, SLOT(handleCredentialsAbsence()));
    connect(m_sso, SIGNAL(credentialsDeleted()),
            this, SLOT(handleCredentialsAbsence()));

    /* Describes a state machine for checking. The rationale for these
    transitions are that
        * we can cancel a check at any time, but this may not necessarily
          propagate to every subsystem. So if a subsystem responds with data
          after a check has been canceled (or completed), it does not start or
          stop a check erroneously.
        * Some parts of the check (e.g. requesting the manifest) should be
          allowed to happen outside a check.
        * While we're waiting for e.g. tokens to download, it's neater to have
          this state as an explicit one, instead of implicit state using
          conditional branching.
    */
    m_transitions[State::Idle]          << State::Manifest
                                        << State::Failed;

    m_transitions[State::Manifest]      << State::Metadata
                                        << State::Complete
                                        << State::Failed
                                        << State::Canceled;

    m_transitions[State::Metadata]      << State::Tokens
                                        << State::TokenComplete
                                        << State::Complete
                                        << State::Failed
                                        << State::Canceled;

    m_transitions[State::Tokens]        << State::TokenComplete
                                        << State::Complete
                                        << State::Failed
                                        << State::Canceled;

    m_transitions[State::TokenComplete] << State::Tokens
                                        << State::Complete
                                        << State::Failed
                                        << State::Canceled;

    m_transitions[State::Complete]      << State::Idle;
    m_transitions[State::Canceled]      << State::Idle;
    m_transitions[State::Failed]        << State::Idle;
}

ManagerImpl::~ManagerImpl()
{
    setState(State::Canceled);
}

void ManagerImpl::setState(const State &state)
{
    if (m_state != state && m_transitions[m_state].contains(state)) {
        m_state = state;
        Q_EMIT stateChanged();
    }
}

ManagerImpl::State ManagerImpl::state() const
{
    return m_state;
}

void ManagerImpl::handleStateChange()
{
    switch (m_state) {
    case State::Idle:
        m_candidates.clear();
        break;
    case State::Manifest:
        m_manifest->request();
        break;
    case State::Metadata:
        requestMetadata();
        break;
    case State::Tokens:
        break;
    case State::TokenComplete:
        completionCheck();
        break;
    case State::Failed:
    case State::Canceled:
        Q_EMIT checkCanceled();
    case State::Complete:
        Q_EMIT checkCompleted();
        setState(State::Idle);
        break;
    }
}

void ManagerImpl::setup(const TokenDownloader *downloader)
{
    connect(downloader, SIGNAL(downloadSucceeded(QSharedPointer<Update>)),
            this, SLOT(handleTokenDownload(QSharedPointer<Update>)));
    connect(downloader, SIGNAL(downloadFailed(QSharedPointer<Update>)),
            this, SLOT(handleTokenDownloadFailure(QSharedPointer<Update>)));
    connect(downloader, SIGNAL(credentialError()),
            this, SLOT(handleCredentialsFailed()));
    connect(this, SIGNAL(checkCanceled()), downloader, SLOT(cancel()));
}

void ManagerImpl::check()
{
    if (checkingForUpdates()) {
        qWarning() << Q_FUNC_INFO << "Check was already in progress.";
        return;
    }

    if (!m_sessionToken->isValid() && !Helpers::isIgnoringCredentials()) {
        qWarning() << Q_FUNC_INFO << "Can't check: invalid session token.";
        m_sso->requestCredentials();
        return;
    }

    setState(State::Manifest);
}

void ManagerImpl::retry(const QString &identifier, const uint &revision)
{
    /* We will not do the token dance here, but rather just create a new signed
    downloadUrl that is valid for 5 minutes. QML will create the actual
    UDM download. */
    auto update = m_model->get(identifier, revision);
    if (update) {
        if (m_sessionToken->isValid() && !Helpers::isIgnoringCredentials()) {
            QString url = m_sessionToken->signUrl(
                update->downloadUrl(), QStringLiteral("GET"), true
            );
            update->setSignedDownloadUrl(
                QString("%1?%2").arg(update->downloadUrl(), url)
            );
            update->setError("");
            update->setState(Update::State::StateAvailable);
        } else {
            qWarning() << Q_FUNC_INFO << "Can't retry: invalid session token.";
            update->setError(SystemSettings::_("Installation failed."));
            update->setState(Update::State::StateFailed);
        }
        update->setProgress(0);
        update->setToken("");
        update->setDownloadId("");
        m_model->update(update);
    }
}

void ManagerImpl::cancel()
{
    setState(State::Canceled);
}

bool ManagerImpl::launch(const QString &identifier)
{
    bool success = false;
    gchar *appId = ubuntu_app_launch_triplet_to_app_id(
        identifier.toLatin1().data(), nullptr, nullptr
    );
    if (appId) {
        success = ubuntu_app_launch_start_application(appId, nullptr);
    }
    g_free(appId);
    return success;
}

void ManagerImpl::handleManifest(const QJsonArray &manifest)
{
    auto updates = parseManifest(manifest);
    synchronize(updates);

    Q_FOREACH(auto update, updates) {
        m_candidates[update->identifier()] = update;
    }
    if (updates.size() == 0) {
        setState(State::Complete);
        return;
    }

    setState(State::Metadata);
}

void ManagerImpl::synchronize(
    const QList<QSharedPointer<Update> > &manifestUpdates
)
{
    /* Runs through all DB updates and if the exist in the manifest, each
    update is “synchronized” with data from the manifest, i.e. marked as
    installed if installed. If not found, this means the app might be
    uninstalled, and we remove the DB entry. */
    auto dbUpdates = m_model->db()->updates();
    Q_FOREACH(auto dbUpdate, dbUpdates) {
        if (dbUpdate->kind() != Update::Kind::KindClick) {
            continue;
        }
        bool found = false;
        Q_FOREACH(auto manifestUpdate, manifestUpdates) {
            /* The local version of a click in the manifest, matched exactly a
            a remote version in one of our db updates. */
            if (manifestUpdate->localVersion() == dbUpdate->remoteVersion()) {
                // We can't know when it was updated, so now() will have to do.
                if (!dbUpdate->updatedAt().isValid()) {
                    dbUpdate->setUpdatedAt(QDateTime::currentDateTimeUtc());
                }
                dbUpdate->setState(Update::State::StateInstallFinished);
                dbUpdate->setInstalled(true);
                dbUpdate->setDownloadId("");
                dbUpdate->setError("");
                m_model->update(dbUpdate);
                found = true;
            } else if (manifestUpdate->identifier() == dbUpdate->identifier()) {
                // Fast forward the local version.
                dbUpdate->setLocalVersion(manifestUpdate->localVersion());

                // Is update in need of update, but at the same time installed?
                if (dbUpdate->isUpdateRequired() && dbUpdate->installed()) {
                    dbUpdate->setInstalled(false);
                    dbUpdate->setState(Update::State::StateAvailable);
                    dbUpdate->setDownloadId("");
                    dbUpdate->setError("");
                    m_model->update(dbUpdate);
                }
                found = true;
            }
        }
        if (!found) {
            m_model->remove(dbUpdate);
        }
    }
}

QList<QSharedPointer<Update> > ManagerImpl::parseManifest(const QJsonArray &manifest)
{
    QList<QSharedPointer<Update> > updates;
    for (int i = 0; i < manifest.size(); i++) {
        const auto object = manifest.at(i).toObject();
        auto name = object.value("name").toString();

        // No name? Bail!
        if (Q_UNLIKELY(name.isEmpty())) {
            continue;
        }

        auto update = QSharedPointer<Update>(new Update);
        update->setIdentifier(name);
        update->setTitle(object.value("title").toString());
        update->setLocalVersion(object.value("version").toString());
        update->setKind(Update::Kind::KindClick);

        /* We'll also need the package name, which will be the first key inside
        the “hooks” key that contains a “desktop”. */
        if (object.contains("hooks") && object.value("hooks").isObject()) {
            auto hooks = object.value("hooks").toObject();
            Q_FOREACH(const auto &key, hooks.keys()) {
                if (hooks[key].isObject() &&
                    hooks[key].toObject().contains("desktop")) {
                    update->setPackageName(key);
                }
            }
        }
        updates << update;
    }
    return updates;
}

void ManagerImpl::completionCheck()
{
    /* Check if tokens are all fetched, or any update has since been marked as
    installed. Return early if that's the case. */
    Q_FOREACH(const auto &identifier, m_candidates.keys()) {
        if (m_candidates[identifier]->token().isEmpty() ||
            m_candidates[identifier]->installed()) {
            setState(State::Tokens);
            return;
        }
    }

    setState(State::Complete);
}

void ManagerImpl::handleTokenDownload(QSharedPointer<Update> update)
{
    auto dl = qobject_cast<TokenDownloader*>(QObject::sender());
    dl->disconnect();

    // Token reported as downloaded, but empty so discard it as a candidate.
    if (update->token().isEmpty()) {
        m_candidates.remove(update->identifier());
    }

    /* Assume the original update was changed during the download of token and
    fetch a new one from the database. */
    auto freshUpdate = m_model->fetch(update);
    if (freshUpdate) {
        freshUpdate->setToken(update->token());
        m_model->add(freshUpdate);
    } else {
        m_model->add(update);
    }

    setState(State::TokenComplete);
    dl->deleteLater();
}

void ManagerImpl::handleTokenDownloadFailure(QSharedPointer<Update> update)
{
    auto dl = qobject_cast<TokenDownloader*>(QObject::sender());

    // Assume the original update was changed during the download of token.
    auto freshUpdate = m_model->get(update);
    if (freshUpdate) {
        freshUpdate->setToken("");
        m_model->add(freshUpdate);
    } else {
        update->setToken("");
        m_model->add(update);
    }

    // We're done with it.
    m_candidates.remove(update->identifier());
    setState(State::TokenComplete);

    dl->deleteLater();
}

void ManagerImpl::handleCredentials(SessionToken *token)
{
    // We'll take ownership of this token.
    m_sessionToken = std::unique_ptr<SessionToken>(token);

    if (!m_sessionToken->isValid() && !Helpers::isIgnoringCredentials()) {
        qWarning() << Q_FUNC_INFO << "Got invalid session token.";
        setAuthenticated(false);
        return;
    }

    setAuthenticated(true);

    cancel();
    check();
}

void ManagerImpl::handleCredentialsAbsence()
{
    m_sessionToken = std::unique_ptr<SessionToken>(new SessionTokenImpl());
    setAuthenticated(false);
    cancel();
}

void ManagerImpl::handleCredentialsFailed()
{
    m_sso->invalidateCredentials();
    m_sessionToken = std::unique_ptr<SessionToken>(new SessionTokenImpl());

    // We've invalidated the token, and the user is now not authenticated.
    setAuthenticated(false);
    cancel();
}

void ManagerImpl::requestMetadata()
{
    QString urlApps = Helpers::clickMetadataUrl();
    QString authHeader;
    if (!Helpers::isIgnoringCredentials()) {
        authHeader = m_sessionToken->signUrl(
            urlApps, QStringLiteral("POST"), true
        );
    }
    QUrl url(urlApps);
    url.setQuery(authHeader);
    m_client->requestMetadata(url, m_candidates.keys());
}

void ManagerImpl::parseMetadata(const QJsonArray &array)
{
    auto now = QDateTime::currentDateTimeUtc();
    for (int i = 0; i < array.size(); i++) {
        auto object = array.at(i).toObject();
        auto identifier = object["name"].toString();
        auto revision = object["revision"].toInt();
        // Check if we already have it's metadata.
        auto dbUpdate = m_model->get(identifier, revision);
        if (dbUpdate) {
            /* If this update is less than 24 hours old (to us), and it has a
            token, we ignore it. */
            if (dbUpdate->createdAt().secsTo(now) <= 86400
                && !dbUpdate->token().isEmpty()) {
                m_candidates.remove(identifier);
                setState(State::TokenComplete);
                continue;
            }
        }

        auto version = object["version"].toString();
        auto icon_url = object["icon_url"].toString();
        auto url = object["download_url"].toString();
        auto download_sha512 = object["download_sha512"].toString();
        auto changelog = object["changelog"].toString();
        auto size = object["binary_filesize"].toInt();
        auto title = object["title"].toString();

        if (m_candidates.contains(identifier)) {
            auto update = m_candidates.value(identifier);
            update->setRemoteVersion(version);

            if (update->isUpdateRequired()) {
                update->setIconUrl(icon_url);
                update->setDownloadUrl(url);
                update->setBinaryFilesize(size);
                update->setDownloadHash(download_sha512);
                update->setChangelog(changelog);
                update->setTitle(title);
                update->setRevision(revision);
                update->setState(Update::State::StateAvailable);

                QStringList command;
                command << Helpers::whichPkcon()
                    << "-p" << "install-local" << "$file";
                update->setCommand(command);

                // Download a token.
                QString signedHeaderUrl(m_sessionToken->signUrl(
                    update->downloadUrl(), QStringLiteral("HEAD"), true
                ));
                auto dl = m_tokenDownloadFactory->create(m_nam, update);
                setup(dl);
                dl->download(signedHeaderUrl);
            } else {
                // Update not required, let's remove it.
                m_candidates.remove(update->identifier());
                setState(State::TokenComplete);
            }
        }
    }

    /* Remove clicks that did not have the necessary metadata. They could be
    locally installed, or removed from upstream. */
    foreach (const auto &identifier, m_candidates.keys()) {
        if (m_candidates.value(identifier)->remoteVersion().isEmpty()) {
            m_candidates.remove(identifier);
            setState(State::TokenComplete);
        }
    }
    setState(State::Tokens);
}

bool ManagerImpl::authenticated() const
{
    return m_authenticated || Helpers::isIgnoringCredentials();
}

bool ManagerImpl::checkingForUpdates() const
{
    return m_state != State::Idle;
}

void ManagerImpl::setAuthenticated(const bool authenticated)
{
    if (authenticated != m_authenticated) {
        m_authenticated = authenticated;
        Q_EMIT authenticatedChanged();
    }
}
} // Click
} // UpdatePlugin
