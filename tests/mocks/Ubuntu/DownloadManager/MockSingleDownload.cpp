/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MockSingleDownload.h"
#include <QDebug>

MockSingleDownload::MockSingleDownload(QObject *parent)
    : QObject(parent)
    , m_errorMessage("")
    , m_downloading(false)
    , m_progress(0)
    , m_metadata(new MockMetadata)
    , m_headers()
    , m_autostart(false)
    , m_hash("")
    , m_algorithm("")
{
    qDebug() << "MockSingleDownload init";
}

void MockSingleDownload::start()
{
    Q_EMIT (started(true));
}

void MockSingleDownload::pause()
{
    Q_EMIT (started(true));

}

void MockSingleDownload::resume()
{
    Q_EMIT (resumed(true));
}

void MockSingleDownload::cancel()
{
    Q_EMIT (canceled(true));

}

void MockSingleDownload::download(QString url)
{
    m_downloading = true;
}

void MockSingleDownload::startDownload()
{

}

bool MockSingleDownload::isCompleted() const
{
    return false;
}

QString MockSingleDownload::errorMessage() const
{
    return m_errorMessage;
}

bool MockSingleDownload::allowMobileDownload() const
{

}

int MockSingleDownload::progress() const
{
    return m_progress;
}

bool MockSingleDownload::downloading() const
{
    return m_downloading;
}

bool MockSingleDownload::downloadInProgress() const
{
    return false;
}

bool MockSingleDownload::autoStart() const
{
    return m_autostart;
}

QString MockSingleDownload::downloadId() const
{
    return "";
}

QVariantMap MockSingleDownload::headers() const
{
    return m_headers;
}

MockMetadata * MockSingleDownload:: metadata()
{
    return m_metadata;
}

void MockSingleDownload::setAllowMobileDownload(bool value)
{

}

void MockSingleDownload::setHeaders(const QVariantMap headers)
{
    m_headers = headers;
    Q_EMIT(metadataChanged());
}

void MockSingleDownload::setMetadata(const MockMetadata * metadata)
{
    m_metadata = const_cast<MockMetadata*>(metadata);
    Q_EMIT(metadataChanged());
}

void MockSingleDownload::setAutoStart(bool value)
{
    m_autostart = value;
}

void MockSingleDownload::mockErrorMessage(const QString &error)
{
    m_errorMessage = error;
    Q_EMIT (errorChanged());
}

void MockSingleDownload::mockFinished()
{
    m_downloading = false;

    Q_EMIT (finished("/path/to/download"));
}

void MockSingleDownload::mockProgress(const int &progress)
{
    m_progress = progress;
    Q_EMIT (progressChanged());
}

void MockSingleDownload::mockDownloading(const bool downloading)
{
    m_downloading = downloading;
    Q_EMIT (downloadingChanged());
}

void MockSingleDownload::mockPause()
{
    m_downloading = false;
    Q_EMIT (paused(true));
}

void MockSingleDownload::mockResume()
{
    m_downloading = true;
    Q_EMIT (resumed(true));
}

void MockSingleDownload::mockProcess()
{
    Q_EMIT (processing("/path/to/download"));
}

void MockSingleDownload::mockStart()
{
    Q_EMIT (started(true));
}

QString MockSingleDownload::hash() const
{
    return m_hash;
}

QString MockSingleDownload::algorithm() const
{
    return m_algorithm;
}

void MockSingleDownload::setHash(const QString &value)
{
    m_hash = value;
    Q_EMIT (hashChanged());
}

void MockSingleDownload::setAlgorithm(const QString &value)
{
    m_algorithm = value;
    Q_EMIT (algorithmChanged());
}


