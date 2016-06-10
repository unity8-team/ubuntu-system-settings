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

}

void MockDownloadManager::mockDownload(MockSingleDownload *download)
{
    m_downloads.append(QVariant::fromValue(download));
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

}
