/*
 * Copyright (C) 2013-2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Class that represents metadata on a click update. This class will talk
 * to the click servers in order to sign the download URL as soon as it is
 * created.
 *
*/
#include "clickupdatemetadata.h"
#include <apt-pkg/debversion.h>
#include "helpers.h"

namespace UpdatePlugin {

ClickUpdateMetadata::ClickUpdateMetadata(QObject *parent):
    ClickApiProto(parent),
    m_clickToken(""),
    m_anonDownloadUrl(""),
    m_binaryFilesize(0),
    m_changelog(""),
    m_channel(""),
    m_content(""),
    m_department(),
    m_downloadSha512(""),
    m_downloadUrl(""),
    m_iconUrl(""),
    m_name(""),
    m_origin(""),
    m_packageName(""),
    m_revision(0),
    m_sequence(0),
    m_status(""),
    m_title(""),
    m_localVersion(""),
    m_remoteVersion("")
{
}

ClickUpdateMetadata::~ClickUpdateMetadata()
{

}

QString ClickUpdateMetadata::anonDownloadUrl() const
{
    return m_anonDownloadUrl;
}

uint ClickUpdateMetadata::binaryFilesize() const
{
    return m_binaryFilesize;
}

QString ClickUpdateMetadata::changelog() const
{
    return m_changelog;
}

QString ClickUpdateMetadata::channel() const
{
    return m_channel;
}

QString ClickUpdateMetadata::content() const
{
    return m_content;
}

QStringList ClickUpdateMetadata::department() const
{
    return m_department;
}

QString ClickUpdateMetadata::downloadSha512() const
{
    return m_downloadSha512;
}

QString ClickUpdateMetadata::downloadUrl() const
{
    return m_downloadUrl;
}

QString ClickUpdateMetadata::iconUrl() const
{
    return m_iconUrl;
}

QString ClickUpdateMetadata::name() const
{
    return m_name;
}

QString ClickUpdateMetadata::origin() const
{
    return m_origin;
}

QString ClickUpdateMetadata::packageName() const
{
    return m_packageName;
}

int ClickUpdateMetadata::revision() const
{
    return m_revision;
}

int ClickUpdateMetadata::sequence() const
{
    return m_sequence;
}

QString ClickUpdateMetadata::status() const
{
    return m_status;
}

QString ClickUpdateMetadata::title() const
{
    return m_title;
}

QString ClickUpdateMetadata::remoteVersion() const
{
    return m_remoteVersion;
}

QString ClickUpdateMetadata::localVersion() const
{
    return m_localVersion;
}

QString ClickUpdateMetadata::clickToken() const
{
    return m_clickToken;
}

void ClickUpdateMetadata::setAnonDownloadUrl(const QString &anonDownloadUrl)
{
    if (m_anonDownloadUrl != anonDownloadUrl) {
        m_anonDownloadUrl = anonDownloadUrl;
        Q_EMIT anonDownloadUrlChanged();
    }
}

void ClickUpdateMetadata::setBinaryFilesize(const uint &binaryFilesize)
{
    if (m_binaryFilesize != binaryFilesize) {
        m_binaryFilesize = binaryFilesize;
        Q_EMIT binaryFilesizeChanged();
    }
}

void ClickUpdateMetadata::setChangelog(const QString &changelog)
{
    if (m_changelog != changelog) {
        m_changelog = changelog;
        Q_EMIT changelogChanged();
    }
}

void ClickUpdateMetadata::setChannel(const QString &channel)
{
    if (m_channel != channel) {
        m_channel = channel;
        Q_EMIT channelChanged();
    }
}

void ClickUpdateMetadata::setContent(const QString &content)
{
    if (m_content != content) {
        m_content = content;
        Q_EMIT contentChanged();
    }
}

void ClickUpdateMetadata::setDepartment(const QStringList &department)
{
    if (m_department != department) {
        m_department = department;
        Q_EMIT departmentChanged();
    }
}

void ClickUpdateMetadata::setDownloadSha512(const QString &downloadSha512)
{
    if (m_downloadSha512 != downloadSha512) {
        m_downloadSha512 = downloadSha512;
        Q_EMIT downloadSha512Changed();
    }
}

void ClickUpdateMetadata::setDownloadUrl(const QString &downloadUrl)
{
    if (m_downloadUrl != downloadUrl) {
        m_downloadUrl = downloadUrl;
        Q_EMIT downloadUrlChanged();
    }
}

void ClickUpdateMetadata::setIconUrl(const QString &iconUrl)
{
    if (m_iconUrl != iconUrl) {
        m_iconUrl = iconUrl;
        Q_EMIT iconUrlChanged();
    }
}

void ClickUpdateMetadata::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged();
    }
}

void ClickUpdateMetadata::setOrigin(const QString &origin)
{
    if (m_origin != origin) {
        m_origin = origin;
        Q_EMIT originChanged();
    }
}

void ClickUpdateMetadata::setPackageName(const QString &packageName)
{
    if (m_packageName != packageName) {
        m_packageName = packageName;
        Q_EMIT packageNameChanged();
    }
}

void ClickUpdateMetadata::setRevision(const int &revision)
{
    if (m_revision != revision) {
        m_revision = revision;
        Q_EMIT revisionChanged();
    }
}

void ClickUpdateMetadata::setSequence(const int &sequence)
{
    if (m_sequence != sequence) {
        m_sequence = sequence;
        Q_EMIT sequenceChanged();
    }
}

void ClickUpdateMetadata::setStatus(const QString &status)
{
    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged();
    }
}

void ClickUpdateMetadata::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        Q_EMIT titleChanged();
    }
}

void ClickUpdateMetadata::setRemoteVersion(const QString &version)
{
    if (m_remoteVersion != version) {
        m_remoteVersion = version;
        Q_EMIT remoteVersionChanged();
    }
}

void ClickUpdateMetadata::setLocalVersion(const QString &version)
{
    if (m_localVersion != version) {
        m_localVersion = version;
        Q_EMIT localVersionChanged();
    }
}

void ClickUpdateMetadata::setClickToken(const QString &clickToken)
{
    qWarning() << "click meta: sat click token" << clickToken;
    if (m_clickToken != clickToken) {
        m_clickToken = clickToken;
        Q_EMIT clickTokenChanged();
    }

    if (!m_clickToken.isEmpty()) {
        qWarning() << "click meta: emit clickTokenRequestSucceeded";
        clickTokenRequestSucceeded(this);
    }
}

void ClickUpdateMetadata::requestClickToken()
{
    qWarning() << "click meta:" << name() << "requests token...";
    if (!m_token.isValid()) {
        qWarning() << "click meta:" << name() << "token invalid";
        Q_EMIT clickTokenRequestFailed();
        return;
    }

    QString authHeader = m_token.signUrl(downloadUrl(), QStringLiteral("HEAD"), true);
    // app->setClickUrl(app->downloadUrl());

    QString signUrl = Helpers::clickTokenUrl(downloadUrl());
    QUrl query(signUrl);
    query.setQuery(authHeader);
    QNetworkRequest request;
    request.setUrl(query);

    QNetworkReply *reply = m_nam.head(request);
    initializeReply(reply);
}

void ClickUpdateMetadata::requestSucceeded(QNetworkReply *reply)
{
    if (reply->hasRawHeader(X_CLICK_TOKEN)) {
        QString header(reply->rawHeader(X_CLICK_TOKEN));
        // This should inform the world that this click update
        // metadata is enough to start a download & install.
        qWarning() << "setting click token to" << header;
        setClickToken(header);
    } else {
        Q_EMIT clickTokenRequestFailed();
    }
    reply->deleteLater();
}

bool ClickUpdateMetadata::isUpdateRequired()
{

    int result = debVS.CmpVersion(m_localVersion.toUtf8().data(),
                                  m_remoteVersion.toUtf8().data());
    return result < 0;
}

// void ClickUpdateMetadata::tokenRequestSslFailed(const QList<QSslError> &errors)
// {

// }

// void ClickUpdateMetadata::tokenRequestFailed(const QNetworkReply::NetworkError &code)
// {

// }

// void ClickUpdateMetadata::tokenRequestSucceeded(const QNetworkReply* reply)
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

} // UpdatePlugin
