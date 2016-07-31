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
#include "update.h"

#include <apt-pkg/debversion.h>
#include <QVariant>

namespace UpdatePlugin
{
Update::Kind Update::kind() const
{
    return m_kind;
}

QString Update::identifier() const
{
    return m_identifier;
}

uint Update::binaryFilesize() const
{
    return m_binaryFilesize;
}

QString Update::changelog() const
{
    return m_changelog;
}

QString Update::channel() const
{
    return m_channel;
}

QDateTime Update::createdAt() const
{
    return m_createdAt;
}

QDateTime Update::updatedAt() const
{
    return m_updatedAt;
}

QString Update::downloadHash() const
{
    return m_downloadHash;
}

QString Update::downloadId() const
{
    return m_downloadId;
}

QString Update::downloadUrl() const
{
    return m_downloadUrl;
}

QString Update::iconUrl() const
{
    return m_iconUrl;
}

bool Update::installed() const
{
    return m_installed;
}

int Update::progress() const
{
    return m_progress;
}

uint Update::revision() const
{
    return m_revision;
}

Update::State Update::state() const
{
    return m_state;
}

QString Update::signedDownloadUrl() const
{
    return m_signedDownloadUrl;
}

QString Update::title() const
{
    return m_title;
}

QString Update::remoteVersion() const
{
    return m_remoteVersion;
}

QString Update::localVersion() const
{
    return m_localVersion;
}

QString Update::token() const
{
    return m_token;
}

QStringList Update::command() const
{
    return m_command;
}

QString Update::error() const
{
    return m_error;
}

QString Update::packageName() const
{
    return m_packageName;
}


void Update::setKind(const Update::Kind &kind)
{
    if (m_kind != kind) {
        m_kind = kind;
        Q_EMIT kindChanged();
    }
}

void Update::setBinaryFilesize(const uint &binaryFilesize)
{
    if (m_binaryFilesize != binaryFilesize) {
        m_binaryFilesize = binaryFilesize;
        Q_EMIT binaryFilesizeChanged();
    }
}

void Update::setChangelog(const QString &changelog)
{
    if (m_changelog != changelog) {
        m_changelog = changelog;
        Q_EMIT changelogChanged();
    }
}

void Update::setChannel(const QString &channel)
{
    if (m_channel != channel) {
        m_channel = channel;
        Q_EMIT channelChanged();
    }
}

void Update::setCreatedAt(const QDateTime &createdAt)
{
    if (m_createdAt != createdAt) {
        m_createdAt = createdAt;
        Q_EMIT createdAtChanged();
    }
}

void Update::setUpdatedAt(const QDateTime &updatedAt)
{
    if (m_updatedAt != updatedAt) {
        m_updatedAt = updatedAt;
        Q_EMIT updatedAtChanged();
    }
}

void Update::setDownloadHash(const QString &downloadHash)
{
    if (m_downloadHash != downloadHash) {
        m_downloadHash = downloadHash;
        Q_EMIT downloadHashChanged();
    }
}

void Update::setDownloadId(const QString &downloadId)
{
    if (m_downloadId != downloadId) {
        m_downloadId = downloadId;
        Q_EMIT downloadIdChanged();
    }
}

void Update::setDownloadUrl(const QString &downloadUrl)
{
    if (m_downloadUrl != downloadUrl) {
        m_downloadUrl = downloadUrl;
        Q_EMIT downloadUrlChanged();
    }
}

void Update::setIconUrl(const QString &iconUrl)
{
    if (m_iconUrl != iconUrl) {
        m_iconUrl = iconUrl;
        Q_EMIT iconUrlChanged();
    }
}

void Update::setIdentifier(const QString &identifier)
{
    if (m_identifier != identifier) {
        m_identifier = identifier;
        Q_EMIT identifierChanged();
    }
}

void Update::setInstalled(const bool installed)
{
    if (m_installed != installed) {
        m_installed = installed;
        Q_EMIT installedChanged();
    }
}

void Update::setProgress(const int &progress)
{
    if (m_progress != progress) {
        m_progress = progress;
        Q_EMIT progressChanged();
    }
}

void Update::setRevision(const uint &revision)
{
    if (m_revision != revision) {
        m_revision = revision;
        Q_EMIT revisionChanged();
    }
}

void Update::setState(const Update::State &state)
{
    if (m_state != state) {
        m_state = state;
        Q_EMIT stateChanged();
    }
}

void Update::setSignedDownloadUrl(const QString &signedDownloadUrl)
{
    if (m_signedDownloadUrl != signedDownloadUrl) {
        m_signedDownloadUrl = signedDownloadUrl;
        Q_EMIT signedDownloadUrlChanged();
    }
}

void Update::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        Q_EMIT titleChanged();
    }
}

void Update::setRemoteVersion(const QString &version)
{
    if (m_remoteVersion != version) {
        m_remoteVersion = version;
        Q_EMIT remoteVersionChanged();
    }
}

void Update::setLocalVersion(const QString &version)
{
    if (m_localVersion != version) {
        m_localVersion = version;
        Q_EMIT localVersionChanged();
    }
}

void Update::setToken(const QString &token)
{
    if (m_token != token) {
        m_token = token;
        Q_EMIT tokenChanged();
    }
}

void Update::setCommand(const QStringList &command)
{
    if (m_command != command) {
        m_command = command;
        Q_EMIT commandChanged();
    }
}

bool Update::automatic() const
{
    return m_automatic;
}

void Update::setAutomatic(const bool automatic)
{
    if (m_automatic != automatic) {
        m_automatic = automatic;
        Q_EMIT automaticChanged();
    }
}

void Update::setError(const QString &error)
{
    if (m_error != error) {
        m_error = error;
        Q_EMIT errorChanged();
    }
}

void Update::setPackageName(const QString &packageName)
{
    if (m_packageName != packageName) {
        m_packageName = packageName;
        Q_EMIT packageNameChanged();
    }
}

bool Update::isUpdateRequired()
{
    int result = debVS.CmpVersion(m_localVersion.toUtf8().data(),
            m_remoteVersion.toUtf8().data());
    return result < 0;
}

bool Update::operator==(const Update &other) const
{
    if (other.identifier() == identifier() && other.revision() == revision())
        return true;
    else
        return false;
}

bool Update::deepEquals(const Update *other) const
{
    if (kind() != other->kind()) return false;
    if (identifier() != other->identifier()) return false;
    if (localVersion() != other->localVersion()) return false;
    if (remoteVersion() != other->remoteVersion()) return false;
    if (revision() != other->revision()) return false;
    if (installed() != other->installed()) return false;
    if (createdAt() != other->createdAt()) return false;
    if (updatedAt() != other->updatedAt()) return false;
    if (title() != other->title()) return false;
    if (downloadHash() != other->downloadHash()) return false;
    if (downloadId() != other->downloadId()) return false;
    if (downloadUrl() != other->downloadUrl()) return false;
    if (binaryFilesize() != other->binaryFilesize()) return false;
    if (iconUrl() != other->iconUrl()) return false;
    if (command() != other->command()) return false;
    if (changelog() != other->changelog()) return false;
    if (token() != other->token()) return false;
    if (state() != other->state()) return false;
    if (signedDownloadUrl() != other->signedDownloadUrl()) return false;
    if (progress() != other->progress()) return false;
    if (automatic() != other->automatic()) return false;
    if (error() != other->error()) return false;
    if (packageName() != other->packageName()) return false;

    return true;
}

bool Update::equals(const Update &other) const
{
    return *this == other;
}

QString Update::stateToString(const Update::State &state)
{
    switch (state) {
    case State::StateUnknown:
        return QLatin1String("unknown");
    case State::StateAvailable:
        return QLatin1String("available");
    case State::StateUnavailable:
        return QLatin1String("unavailable");
    case State::StateQueuedForDownload:
        return QLatin1String("queuedfordownload");
    case State::StateDownloading:
        return QLatin1String("downloading");
    case State::StateDownloadingAutomatically:
        return QLatin1String("downloadingautomatically");
    case State::StateDownloadPaused:
        return QLatin1String("downloadpaused");
    case State::StateAutomaticDownloadPaused:
        return QLatin1String("automaticdownloadpaused");
    case State::StateInstalling:
        return QLatin1String("installing");
    case State::StateInstallingAutomatically:
        return QLatin1String("installingautomatically");
    case State::StateInstallPaused:
        return QLatin1String("installpaused");
    case State::StateInstallFinished:
        return QLatin1String("installfinished");
    case State::StateInstalled:
        return QLatin1String("installed");
    case State::StateDownloaded:
        return QLatin1String("downloaded");
    case State::StateFailed:
        return QLatin1String("failed");
    }
    return QLatin1String("unknown");
}

Update::State Update::stringToState(const QString &state)
{
    if (state == QLatin1String("available"))
        return State::StateAvailable;
    if (state == QLatin1String("unavailable"))
        return State::StateUnavailable;
    if (state == QLatin1String("queuedfordownload"))
        return State::StateQueuedForDownload;
    if (state == QLatin1String("downloading"))
        return State::StateDownloading;
    if (state == QLatin1String("downloadingautomatically"))
        return State::StateDownloadingAutomatically;
    if (state == QLatin1String("downloadpaused"))
        return State::StateDownloadPaused;
    if (state == QLatin1String("automaticdownloadpaused"))
        return State::StateAutomaticDownloadPaused;
    if (state == QLatin1String("installing"))
        return State::StateInstalling;
    if (state == QLatin1String("installingautomatically"))
        return State::StateInstallingAutomatically;
    if (state == QLatin1String("installpaused"))
        return State::StateInstallPaused;
    if (state == QLatin1String("installfinished"))
        return State::StateInstallFinished;
    if (state == QLatin1String("installed"))
        return State::StateInstalled;
    if (state == QLatin1String("downloaded"))
        return State::StateDownloaded;
    if (state == QLatin1String("failed"))
        return State::StateFailed;

    return State::StateUnknown;
}

QString Update::kindToString(const Update::Kind &kind)
{
    switch (kind) {
    case Kind::KindClick:
        return QLatin1String("click");
    case Kind::KindImage:
        return QLatin1String("image");
    case Kind::KindUnknown:
    default:
        return QLatin1String("unknown");
    }
}

Update::Kind Update::stringToKind(const QString &kind)
{
    if (kind == QLatin1String("click"))
        return Kind::KindClick;
    if (kind == QLatin1String("image"))
        return Kind::KindImage;

    return Kind::KindUnknown;
}
} // UpdatePlugin
