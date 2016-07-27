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

#include <ubuntu-app-launch.h>

#include <QDateTime>
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
ManagerImpl::ManagerImpl(UpdateModel *model, QObject *parent)
    : Manager(parent)
    , m_client(new Click::ClientImpl(this))
    , m_manifest(new Click::ManifestImpl(this))
    , m_sso(new Click::SSOImpl(this))
    , m_downloadFactory(new Click::TokenDownloaderFactoryImpl)
    , m_model(model)
{
    init();
}

ManagerImpl::ManagerImpl(Click::Client *client,
                 Click::Manifest *manifest,
                 Click::SSO *sso,
                 Click::TokenDownloaderFactory *downloadFactory,
                 UpdateModel *model,
                 QObject *parent)
    : Manager(parent)
    , m_client(client)
    , m_manifest(manifest)
    , m_sso(sso)
    , m_downloadFactory(downloadFactory)
    , m_model(model)
{
    init();
}

void ManagerImpl::init()
{
    initClient();

    initManifest();
    /* We use the manifest as a source of information on what state apps are
    in. I.e. if we have a pending update's remote version matching that of an
    app in the manifest; we can make assumptions about that update's state. */
    m_manifest->request();

    initSSO();

    connect(this, SIGNAL(checkStarted()), this, SLOT(handleCheckStart()));
    connect(this, SIGNAL(checkCompleted()), this, SLOT(handleCheckStop()));
    connect(this, SIGNAL(checkFailed()), this, SLOT(handleCheckStop()));
    connect(this, SIGNAL(checkCanceled()), this, SLOT(handleCheckStop()));
}

ManagerImpl::~ManagerImpl()
{
    delete m_downloadFactory;
}

void ManagerImpl::initTokenDownloader(const Click::TokenDownloader *downloader)
{
    connect(downloader, SIGNAL(downloadSucceeded(QSharedPointer<Update>)),
            this, SLOT(handleTokenDownload(QSharedPointer<Update>)));
    connect(downloader, SIGNAL(downloadFailed(QSharedPointer<Update>)),
            this, SLOT(handleTokenDownloadFailure(QSharedPointer<Update>)));
    connect(this, SIGNAL(checkCanceled()), downloader, SLOT(cancel()));
}

void ManagerImpl::initClient()
{
    connect(m_client, SIGNAL(metadataRequestSucceeded(const QJsonArray&)),
            this, SLOT(parseMetadata(const QJsonArray&)));
    connect(m_client, SIGNAL(serverError()),
            this, SLOT(handleCommunicationErrors()));
    connect(m_client, SIGNAL(networkError()),
            this, SLOT(handleCommunicationErrors()));
    connect(m_client, SIGNAL(credentialError()),
            this, SLOT(handleCommunicationErrors()));

    // Redirect signals to consumers.
    connect(m_client, SIGNAL(networkError()),
            this, SIGNAL(networkError()));
    connect(m_client, SIGNAL(serverError()),
            this, SIGNAL(serverError()));
    connect(m_client, SIGNAL(credentialError()),
            this, SIGNAL(credentialError()));
}

void ManagerImpl::initManifest()
{
    connect(m_manifest, SIGNAL(requestSucceeded(const QJsonArray&)),
            this, SLOT(handleManifest(const QJsonArray&)));
    connect(m_manifest, SIGNAL(requestFailed()),
            this, SLOT(handleManifestFailure()));
}

void ManagerImpl::initSSO()
{
    connect(m_sso, SIGNAL(credentialsRequestSucceeded(const UbuntuOne::Token&)),
            this, SLOT(handleCredentials(const UbuntuOne::Token&)));
    connect(m_sso, SIGNAL(credentialsRequestFailed()),
            this, SLOT(handleCredentialsFailed()));
}

void ManagerImpl::check()
{
    if (m_checking) {
        qWarning() << Q_FUNC_INFO << "Check was already in progress.";
        return;
    }

    // Don't check for click updates if there are no credentials,
    // instead ask for credentials.
    if (!m_authToken.isValid() && !Helpers::isIgnoringCredentials()) {
        m_sso->requestCredentials();
        return;
    }

    setCheckingForUpdates(true);
    m_candidates.clear();
    m_manifest->request();
}

void ManagerImpl::retry(const QString &identifier, const uint &revision)
{
    /* We'll simply ask for another click token if a click update failed
    to install. */
    QSharedPointer<Update> u = m_model->get(identifier, revision);
    if (u->identifier() == identifier && u->revision() == revision) {
        m_model->setAvailable(identifier, revision, true);

        Click::TokenDownloader* dl = m_downloadFactory->create(u);

        dl->setAuthToken(m_authToken);
        initTokenDownloader(dl);
        dl->download();
    }
}

void ManagerImpl::cancel()
{
    m_client->cancel();
    setCheckingForUpdates(false);
}

void ManagerImpl::launch(const QString &identifier, const uint &revision)
{
    // if (!ubuntu_app_launch_start_application(appId.toLatin1().data(), nullptr)) {
    //     qWarning() << Q_FUNC_INFO << "Could not launch app" << appId;
    // }
}

void ManagerImpl::handleManifest(const QJsonArray &manifest)
{
    QList<QSharedPointer<Update> > updates = parseManifest(manifest);

    Q_FOREACH(const QSharedPointer<Update> &update, updates) {
        QSharedPointer<Update> existingUpdate = m_model->get(
            update->identifier(), update->localVersion()
        );
        if (!existingUpdate.isNull()) {
            m_model->setInstalled(existingUpdate->identifier(), existingUpdate->revision());
        }
    }

    if (m_checking) {
        if (updates.size() == 0) {
            Q_EMIT checkCompleted();
            setCheckingForUpdates(false);
            return;
        }

        Q_FOREACH(QSharedPointer<Update> update, updates) {
            m_candidates[update->identifier()] = update;
        }

        requestMetadata();
    }
}

void ManagerImpl::handleManifestFailure()
{
    if (m_checking)
        setCheckingForUpdates(false);
}

QList<QSharedPointer<Update> > ManagerImpl::parseManifest(const QJsonArray &manifest)
{
    QList<QSharedPointer<Update> > updates;
    for (int i = 0; i < manifest.size(); i++) {
        QSharedPointer<Update> update = QSharedPointer<Update>(new Update);

        const QJsonObject object = manifest.at(i).toObject();
        update->setIdentifier(object.value("name").toString());
        update->setTitle(object.value("title").toString());
        update->setLocalVersion(object.value("version").toString());
        update->setKind(Update::Kind::KindClick);

        /* We'll also need the package name, which will be the first key inside
        the “hooks” key that contains a “desktop”.. */
        if (object.contains("hooks") && object.value("hooks").isObject()) {
            QJsonObject hooks = object.value("hooks").toObject();
            Q_FOREACH(const QString &key, hooks.keys()) {
                if (hooks[key].isObject() &&
                    hooks[key].toObject().contains("desktop")) {
                    update->setPackageName(key);
                }
            }
        }

        QStringList command;
        command << Helpers::whichPkcon() << "-p" << "install-local" << "$file";
        update->setCommand(command);
        updates << update;
    }
    return updates;
}

void ManagerImpl::handleTokenDownload(QSharedPointer<Update> update)
{
    Click::TokenDownloader* dl = qobject_cast<Click::TokenDownloader*>(QObject::sender());
    dl->disconnect();

    // Token reported as downloaded, but empty so discard it as a candidate.
    if (update->token().isEmpty()) {
        m_candidates.remove(update->identifier());
    }

    /* Assume the original update was changed during the download of token and
    fetch a new one from the database. */
    QSharedPointer<Update> freshUpdate = m_model->fetch(update->identifier(),
                                                        update->revision());
    if (freshUpdate) {
        freshUpdate->setToken(update->token());
        m_model->add(freshUpdate);
    } else {
        m_model->add(update);
    }

    dl->deleteLater();
    completionCheck();
}

void ManagerImpl::completionCheck()
{
    /* Check if tokens are all fetched, or any update has since been marked as
    installed. Return early if that's the case. */
    Q_FOREACH(const QString &identifier, m_candidates.keys()) {
        if (m_candidates[identifier]->token().isEmpty() ||
            m_candidates[identifier]->installed()) {
            return;
        }
    }

    // All updates had tokens, check is complete.
    Q_EMIT checkCompleted();
    setCheckingForUpdates(false);
}

void ManagerImpl::handleTokenDownloadFailure(QSharedPointer<Update> update)
{
    Click::TokenDownloader* dl = qobject_cast<Click::TokenDownloader*>(QObject::sender());
    dl->disconnect();

    // Assume the original update was changed during the download of token.
    QSharedPointer<Update> freshUpdate = m_model->get(update->identifier(),
                                                      update->revision());
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
    completionCheck();
}

void ManagerImpl::handleCredentials(const UbuntuOne::Token &token)
{
    m_authToken = token;

    if (!m_authToken.isValid() && !Helpers::isIgnoringCredentials()) {
        handleCredentialsFailed();
        return;
    }

    setAuthenticated(true);

    cancel();
    check();
}

void ManagerImpl::handleCredentialsFailed()
{
    m_sso->invalidateCredentials();
    m_authToken = UbuntuOne::Token();

    cancel();

    // We've invalidated the token, and the user is now not authenticated.
    setAuthenticated(false);
}

void ManagerImpl::handleCommunicationErrors()
{
    if (m_checking)
        setCheckingForUpdates(false);
}

void ManagerImpl::requestMetadata()
{
    QList<QString> packages;
    Q_FOREACH(const QString &identifier, m_candidates.keys()) {
        packages.append(identifier);
    }

    QString urlApps = Helpers::clickMetadataUrl();

    QString authHeader;
    if (!Helpers::isIgnoringCredentials()) {
        authHeader = m_authToken.signUrl(
            urlApps, QStringLiteral("POST"), true
        );
    }
    QUrl url(urlApps);
    url.setQuery(authHeader);

    m_client->requestMetadata(url, packages);
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
            QSharedPointer<Update> update = m_candidates.value(identifier);
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

                Click::TokenDownloader* dl = m_downloadFactory->create(update);
                dl->setAuthToken(m_authToken);
                initTokenDownloader(dl);
                dl->download();
            } else {
                // Update not required, let's remove it.
                m_candidates.remove(update->identifier());
                completionCheck();
            }
        }
    }

    /* Remove clicks that did not have the necessary metadata. They could be
    locally installed, or removed from upstream. */
    foreach (const QString &identifier, m_candidates.keys()) {
        if (m_candidates.value(identifier)->remoteVersion().isEmpty())
            m_candidates.remove(identifier);
    }
    completionCheck();
}

bool ManagerImpl::authenticated() const
{
    return m_authenticated || Helpers::isIgnoringCredentials();
}

bool ManagerImpl::checkingForUpdates() const
{
    return m_checking;
}

void ManagerImpl::setAuthenticated(const bool authenticated)
{
    if (authenticated != m_authenticated) {
        m_authenticated = authenticated;
        Q_EMIT authenticatedChanged();
    }
}

void ManagerImpl::setCheckingForUpdates(const bool checking)
{
    if (checking != m_checking) {
        m_checking = checking;
        Q_EMIT checkingForUpdatesChanged();
    }
}
} // Click
} // UpdatePlugin
