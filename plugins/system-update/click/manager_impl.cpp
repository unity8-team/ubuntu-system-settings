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

#include "click/manager_impl.h"
#include "click/client_impl.h"
#include "click/manifest_impl.h"
#include "click/sso_impl.h"
#include "click/tokendownloader_factory_impl.h"

#include "network/accessmanager_impl.h"

#include <ubuntu-app-launch.h>

#include <QDateTime>
#include <QFinalState>
#include <QState>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>

// FIXME: need to do this better including #include "../../src/i18n.h"
// and linking to it
#include <libintl.h>
QString _(const char *text)
{
    return QString::fromUtf8(dgettext(0, text));
}

namespace UpdatePlugin
{
namespace Click
{
ManagerImpl::ManagerImpl(UpdateModel *model, Network::Manager *nam,
                         QObject *parent)
    : ManagerImpl(model,
                  nam,
                  new Click::ClientImpl(nam),
                  new Click::ManifestImpl(),
                  new Click::SSOImpl(),
                  new Click::TokenDownloaderFactoryImpl,
                  parent)
{
    m_client->setParent(this);
    m_manifest->setParent(this);
    m_sso->setParent(this);
}

ManagerImpl::ManagerImpl(UpdateModel *model,
                         Network::Manager *nam,
                         Click::Client *client,
                         Click::Manifest *manifest,
                         Click::SSO *sso,
                         Click::TokenDownloaderFactory *tokenDownloadFactory,
                         QObject *parent)
    : Manager(parent)
    , m_model(model)
    , m_nam(nam)
    , m_client(client)
    , m_manifest(manifest)
    , m_sso(sso)
    , m_tokenDownloadFactory(tokenDownloadFactory)
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
    connect(m_client, &Client::serverError, this, [this]() {
        setState(State::Failed);
    });
    connect(m_client, &Client::networkError, this, [this]() {
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
    connect(m_sso,
            SIGNAL(credentialsFound(const UbuntuOne::Token&)),
            this, SLOT(handleCredentials(const UbuntuOne::Token&)));
    connect(m_sso, SIGNAL(credentialsNotFound()),
            this, SLOT(handleCredentialsAbsence()));
    connect(m_sso, SIGNAL(credentialsDeleted()),
            this, SLOT(handleCredentialsAbsence()));

    /* Describes a state machine for checking. The rationale for these
    transitions are that
        * we can cancel a check at any time, but this may not necessarily
          propagate to every subsystem. So if a subsystem responds with data
          after a check has been canceled (or completed), it is a noop as far
          as checking is concerned.
        * Some parts of the check (e.g. requesting the manifest) should be
          allowed to happen outside a check.
        * While we're waiting for e.g. tokens to download, it's neater to have
          this state as an explicit one, instead of waiting for a queue
          to empty.
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
    delete m_tokenDownloadFactory;
}

void ManagerImpl::setState(const State &state)
{
    if (!m_transitions[m_state].contains(state)) {
        qWarning() << "illegal transition from" << (int) m_state << "to" << (int) state;
    }

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
        qWarning() << "changed to State::Idle";
        m_candidates.clear();
        break;
    case State::Manifest:
        qWarning() << "changed to State::Manifest";
        m_manifest->request();
        break;
    case State::Metadata:
        qWarning() << "changed to State::Metadata";
        requestMetadata();
        break;
    case State::Tokens:
        qWarning() << "changed to State::Tokens";
        break;
    case State::TokenComplete:
        qWarning() << "changed to State::TokenComplete";
        completionCheck();
        break;
    case State::Failed:
        qWarning() << "changed to State::Failed";
        Q_EMIT checkCanceled();
        setState(State::Idle);
        break;
    case State::Complete:
        qWarning() << "changed to State::Complete";
        Q_EMIT checkCompleted();
        setState(State::Idle);
        break;
    case State::Canceled:
        qWarning() << "changed to State::Canceled";
        Q_EMIT checkCanceled();
        setState(State::Idle);
        break;
    }
}

void ManagerImpl::setup(const Click::TokenDownloader *downloader)
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

    // Don't check for click updates if there are no credentials,
    // instead ask for credentials.
    if (!m_authToken.isValid() && !Helpers::isIgnoringCredentials()) {
        qWarning() << Q_FUNC_INFO << "Credential invalid.";
        setAuthenticated(false);
        m_sso->requestCredentials();
        return;
    }

    setState(State::Manifest);
}

void ManagerImpl::retry(const QString &identifier, const uint &revision)
{
    /* We'll simply ask for another click token if a click update failed
    to install. */
    auto update = m_model->get(identifier, revision);
    if (update) {
        m_model->setAvailable(identifier, revision, true);
        auto dl = m_tokenDownloadFactory->create(m_nam, update);
        dl->setAuthToken(m_authToken);
        setup(dl);
        dl->download();
    }
}

void ManagerImpl::cancel()
{
    setState(State::Canceled);
}

void ManagerImpl::launch(const QString &identifier, const uint &revision)
{
    // if (!ubuntu_app_launch_start_application(appId.toLatin1().data(), nullptr)) {
    //     qWarning() << Q_FUNC_INFO << "Could not launch app" << appId;
    // }
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
                m_model->setInstalled(dbUpdate->identifier(),
                                      dbUpdate->revision());
                found = true;
            } else if (manifestUpdate->identifier() == dbUpdate->identifier()) {
                // Fast forward the local version.
                dbUpdate->setLocalVersion(manifestUpdate->localVersion());
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
        if (name.isEmpty()) {
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
    auto dl = qobject_cast<Click::TokenDownloader*>(QObject::sender());
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

    dl->deleteLater();
    setState(State::TokenComplete);
}

void ManagerImpl::handleTokenDownloadFailure(QSharedPointer<Update> update)
{
    auto dl = qobject_cast<Click::TokenDownloader*>(QObject::sender());
    dl->disconnect();

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
    dl->deleteLater();

    setState(State::TokenComplete);
}

void ManagerImpl::handleCredentials(const UbuntuOne::Token &token)
{
    qWarning() << Q_FUNC_INFO;
    m_authToken = token;

    if (!m_authToken.isValid() && !Helpers::isIgnoringCredentials()) {
        qWarning() << Q_FUNC_INFO << "not valid";
        handleCredentialsFailed();
        return;
    }

    qWarning() << Q_FUNC_INFO << "auth true";
    setAuthenticated(true);

    cancel();
    check();
}

void ManagerImpl::handleCredentialsAbsence()
{
    setAuthenticated(false);
    cancel();
}

void ManagerImpl::handleCredentialsFailed()
{
    qWarning() << Q_FUNC_INFO;
    m_sso->invalidateCredentials();
    m_authToken = UbuntuOne::Token();

    // We've invalidated the token, and the user is now not authenticated.
    setAuthenticated(false);
    qWarning() << Q_FUNC_INFO << "auth false";
    cancel();
}

void ManagerImpl::requestMetadata()
{
    QString urlApps = Helpers::clickMetadataUrl();
    QString authHeader;
    if (!Helpers::isIgnoringCredentials()) {
        authHeader = m_authToken.signUrl(
            urlApps, QStringLiteral("POST"), true
        );
    }
    QUrl url(urlApps);
    url.setQuery(authHeader);
    m_client->requestMetadata(url, m_candidates.keys());
}

void ManagerImpl::parseMetadata(const QJsonArray &array)
{
    for (int i = 0; i < array.size(); i++) {
        auto object = array.at(i).toObject();
        auto identifier = object["name"].toString();
        auto version = object["version"].toString();
        auto icon_url = object["icon_url"].toString();
        auto url = object["download_url"].toString();
        auto download_sha512 = object["download_sha512"].toString();
        auto changelog = object["changelog"].toString();
        auto size = object["binary_filesize"].toInt();
        auto title = object["title"].toString();
        auto revision = object["revision"].toInt();
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

                Click::TokenDownloader* dl
                    = m_tokenDownloadFactory->create(m_nam, update);
                dl->setAuthToken(m_authToken);
                setup(dl);
                dl->download();
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
