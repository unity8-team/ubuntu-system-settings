/*
 * Copyright (C) 2016 Canonical Ltd
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
*/
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "clickapicache.h"

namespace UpdatePlugin {

const QString ClickApiCache::CACHED_AT(QStringLiteral("cached_at"));
const QString ClickApiCache::METADATAS(QStringLiteral("metadatas"));
const QString ClickApiCache::NAME(QStringLiteral("name"));
const QString ClickApiCache::REMOTE_VERSION(QStringLiteral("remote_version"));
const QString ClickApiCache::LOCAL_VERSION(QStringLiteral("local_version"));
const QString ClickApiCache::ICON_URL(QStringLiteral("icon_url"));
const QString ClickApiCache::DOWNLOAD_URL(QStringLiteral("download_url"));
const QString ClickApiCache::DOWNLOAD_SHA512(QStringLiteral("download_sha512"));
const QString ClickApiCache::CHANGELOG(QStringLiteral("changelog"));
const QString ClickApiCache::BINARY_FILESIZE(QStringLiteral("binary_filesize"));
const QString ClickApiCache::TITLE(QStringLiteral("title"));
const QString ClickApiCache::CLICK_TOKEN(QStringLiteral("click_token"));

ClickApiCache::ClickApiCache(QObject *parent, const QString &file):
    QObject(parent),
    m_cache(file)
{
}

ClickApiCache::~ClickApiCache()
{
}

bool ClickApiCache::valid()
{
    bool val = false;
    if (!m_cache.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "could not open cache file," << m_cache.errorString();
    } else {
        QString cachedJson = m_cache.readAll();
        QJsonDocument d = QJsonDocument::fromJson(cachedJson.toUtf8());
        QJsonObject o = d.object();

        // How old is this cache?
        int cachedAt = o.value(CACHED_AT).toInt(0);
        uint now = QDateTime::currentDateTime().toTime_t();

        // If the cache timestamp is older than 24 hours, don't use it.
        qWarning() << "click cache:" << cachedAt << now << (now - 86400);
        val = cachedAt > (now - 86400);
    }
    m_cache.close();
    qWarning() << "click cache: returning val" << val;
    return val;
}

QList<QSharedPointer<ClickUpdateMetadata> > ClickApiCache::read()
{
    QList<QSharedPointer<ClickUpdateMetadata> > list;

    if (!m_cache.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "could not open cache file," << m_cache.errorString();
    } else {
        QString cachedJson = m_cache.readAll();
        QJsonDocument d = QJsonDocument::fromJson(cachedJson.toUtf8());
        foreach(const QJsonValue &v, d.object()[METADATAS].toArray()) {
            QSharedPointer<ClickUpdateMetadata> meta(new ClickUpdateMetadata);
            if (v.isObject() && parse(v.toObject(), meta)) {
                list.append(meta);
            }
        }
    }

    m_cache.close();
    return list;
}

void ClickApiCache::write(const QList<QSharedPointer<ClickUpdateMetadata> > &metas)
{
    if (!m_cache.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "could not open cache file for writing," << m_cache.errorString();
        return;
    }

    QJsonObject root;
    root.insert(METADATAS, serialize(metas));

    QJsonValue now((int) QDateTime::currentDateTime().toTime_t());
    root.insert(CACHED_AT, now);
    QJsonDocument d(root);

    if (m_cache.write(d.toJson()) > 0) {
        qWarning() << "successfully wrote to cache";
    } else {
        qWarning() << "failed to write to cache";
    }
    m_cache.close();
}

bool ClickApiCache::parse(const QJsonObject &obj, const QSharedPointer<ClickUpdateMetadata> &meta)
{
    // If either of the following values cannot be found, the click update
    // metadata is incomplete and the user needs to fetch new data from
    // a server.
    QString name = obj[NAME].toString();
    qWarning() << "click cache: parsing" << name << "...";
    QString localVersion = obj[LOCAL_VERSION].toString();
    QString remoteVersion = obj[REMOTE_VERSION].toString();
    QString iconUrl = obj[ICON_URL].toString();
    QString downloadUrl = obj[DOWNLOAD_URL].toString();
    QString downloadSha512 = obj[DOWNLOAD_SHA512].toString();
    QString changelog = obj[CHANGELOG].toString();
    int size = obj[BINARY_FILESIZE].toInt();
    QString title = obj[TITLE].toString();
    QString clickToken = obj[CLICK_TOKEN].toString();

    if (remoteVersion.isEmpty() || localVersion.isEmpty() || iconUrl.isEmpty()
        || downloadUrl.isEmpty() || downloadSha512.isEmpty()
        || changelog.isEmpty() || title.isEmpty() || clickToken.isEmpty())
        return false;

    meta->setName(name);
    meta->setRemoteVersion(remoteVersion);
    meta->setLocalVersion(localVersion);
    meta->setIconUrl(iconUrl);
    meta->setDownloadUrl(downloadUrl);
    meta->setDownloadSha512(downloadSha512);
    meta->setChangelog(changelog);
    meta->setBinaryFilesize(size);
    meta->setTitle(title);
    meta->setClickToken(clickToken);
    return true;
}

QJsonArray ClickApiCache::serialize(
    const QList<QSharedPointer<ClickUpdateMetadata>> &metas)
{
    QJsonArray a;
    foreach(const QSharedPointer<ClickUpdateMetadata> &meta, metas) {
        QJsonObject o;
        o.insert(NAME, meta->name());
        o.insert(REMOTE_VERSION, meta->remoteVersion());
        o.insert(LOCAL_VERSION, meta->localVersion());
        o.insert(ICON_URL, meta->iconUrl());
        o.insert(DOWNLOAD_URL, meta->downloadUrl());
        o.insert(DOWNLOAD_SHA512, meta->downloadSha512());
        o.insert(CHANGELOG, meta->changelog());
        o.insert(BINARY_FILESIZE, (int) meta->binaryFilesize());
        o.insert(TITLE, meta->title());
        o.insert(CLICK_TOKEN, meta->clickToken());
        a.append(QJsonValue(o));
    }
    return a;
}

} // UpdatePlugin
