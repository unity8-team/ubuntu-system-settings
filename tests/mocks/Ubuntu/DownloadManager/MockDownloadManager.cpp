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

#include "MockDownloadManager.h"
#include <QDebug>

MockDownloadManager::MockDownloadManager(QObject *parent)
    : QObject(parent)
    , m_downloads()
{
}

MockDownloadManager::~MockDownloadManager()
{
}

void MockDownloadManager::download(QString url)
{
    Q_UNUSED(url)
}

QVariantList MockDownloadManager::downloads()
{
    return m_downloads;
}

QString MockDownloadManager::errorMessage() const
{
    return QString();
}

bool MockDownloadManager::autoStart() const
{
    return false;
}

void MockDownloadManager::setAutoStart(bool value)
{
    Q_UNUSED(value)
}

void MockDownloadManager::mockDownload(MockSingleDownload *download)
{
    m_downloads.append(QVariant::fromValue(download));
    Q_EMIT downloadsChanged();
}

void MockDownloadManager::mockDownloadFinished(MockSingleDownload *download,
                                               const QString &path)
{
    m_downloads.removeOne(QVariant::fromValue(download));
    Q_EMIT downloadFinished(download, path);
}

void MockDownloadManager::mockDownloadPaused(MockSingleDownload *download)
{
    Q_EMIT downloadPaused(download);
}

void MockDownloadManager::mockDownloadResumed(MockSingleDownload *download)
{
    Q_EMIT downloadResumed(download);
}

void MockDownloadManager::mockDownloadCanceled(MockSingleDownload *download)
{
    Q_EMIT downloadCanceled(download);
}

void MockDownloadManager::mockErrorFound(MockSingleDownload *download,
                                         const QString &error)
{
    download->mockErrorMessage(error);
    Q_EMIT errorFound(download);
}
