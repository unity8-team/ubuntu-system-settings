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

#ifndef CLICKAPICACHE_H
#define CLICKAPICACHE_H

#include <QFile>
#include <QList>
#include <QSharedPointer>
#include <QStandardPaths>
#include <QString>

#include "clickupdatemetadata.h"

namespace UpdatePlugin
{

class ClickUpdateMetadata;

/**
 Caches a list of ClickUpdateMetadatas for 24 hours.
 */
class ClickApiCache: public QObject
{
Q_OBJECT
public:
    explicit ClickApiCache(
            QObject *parent = 0,
            const QString &file = QStandardPaths::writableLocation(
                    QStandardPaths::CacheLocation)
                    + "/click-metadata-cache.json");
    ~ClickApiCache();
    static const QString CACHED_AT;
    static const QString METADATAS;
    static const QString NAME;
    static const QString LOCAL_VERSION;
    static const QString REMOTE_VERSION;
    static const QString ICON_URL;
    static const QString DOWNLOAD_URL;
    static const QString DOWNLOAD_SHA512;
    static const QString CHANGELOG;
    static const QString BINARY_FILESIZE;
    static const QString TITLE;
    static const QString CLICK_TOKEN;

    // Return whether or not the cache is valid.
    bool valid();

    // Return a list of ClickUpdateMetadatas from cache.
    QList<QSharedPointer<ClickUpdateMetadata> > read();

    // Set the cache to be a list of ClickUpdateMetadatas.
    void write(const QList<QSharedPointer<ClickUpdateMetadata> > &metas);

private:
    // Parse a JSON object, and if successful, update the
    // ClickUpdateMetadata with values from the JSON object.
    bool parse(const QJsonObject &obj,
               const QSharedPointer<ClickUpdateMetadata> &meta);

    // Serialize a ClickUpdateMetadata object.
    QJsonArray serialize(
            const QList<QSharedPointer<ClickUpdateMetadata>> &metas);

    QFile m_cache;
};

} // UpdatePlugin

#endif // CLICKAPICACHE_H
