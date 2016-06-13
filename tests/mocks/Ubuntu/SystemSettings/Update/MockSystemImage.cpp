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

#include "MockSystemImage.h"

MockSystemImage::MockSystemImage(QObject *parent)
    : QObject(parent)
    , m_downloadMode(-1)
{
}

MockSystemImage::~MockSystemImage()
{
}

void MockSystemImage::factoryReset()
{
}

void MockSystemImage::productionReset()
{
}

void MockSystemImage::checkForUpdate()
{
}

void MockSystemImage::downloadUpdate()
{
}

void MockSystemImage::forceAllowGSMDownload()
{
}

void MockSystemImage::applyUpdate()
{
}

void MockSystemImage::cancelUpdate()
{
}

void MockSystemImage::pauseDownload()
{
}

bool MockSystemImage::getIsTargetNewer() const
{
    return false;
}

QString MockSystemImage::deviceName() const
{
    return QString();
}

QString MockSystemImage::channelName() const
{
    return QString();
}

QDateTime MockSystemImage::lastUpdateDate() const
{
    return QDateTime();
}

QDateTime MockSystemImage::lastCheckDate() const
{
    return QDateTime();
}

bool MockSystemImage::updateAvailable()
{
    return false;
}

bool MockSystemImage::downloading()
{
    return false;
}

QString MockSystemImage::availableVersion()
{
    return QString();
}

int MockSystemImage::updateSize()
{
    return 0;
}

QString MockSystemImage::errorReason()
{
    return QString();
}

int MockSystemImage::currentBuildNumber() const
{
    return 0;
}

QString MockSystemImage::currentUbuntuBuildNumber() const
{
    return QString();
}

QString MockSystemImage::currentDeviceBuildNumber() const
{
    return QString();
}

QString MockSystemImage::currentCustomBuildNumber() const
{
    return QString();
}

int MockSystemImage::targetBuildNumber() const
{
    return 0;
}

QVariantMap MockSystemImage::detailedVersionDetails() const
{
    return QVariantMap();
}

int MockSystemImage::downloadMode()
{
    return m_downloadMode;
}

void MockSystemImage::setDownloadMode(const int &downloadMode)
{
    m_downloadMode = downloadMode;
    Q_EMIT (downloadModeChanged());
}

void MockSystemImage::mockProgress(const int &percentage, const double &eta)
{
    Q_EMIT (updateProgress(percentage, eta));
}


void MockSystemImage::mockAvailableStatus(const bool isAvailable,
                                          const bool downloading,
                                          const QString availableVersion,
                                          const int updateSize,
                                          const QString lastUpdateDate,
                                          const QString errorReason)
{
    Q_EMIT (updateAvailableStatus(isAvailable,
                                  downloading,
                                  availableVersion,
                                  updateSize,
                                  lastUpdateDate,
                                  errorReason));
}

void MockSystemImage::mockPaused(const int &percentage)
{
    Q_EMIT (updatePaused(percentage));
}

void MockSystemImage::mockStarted()
{
    Q_EMIT (downloadStarted());
}

void MockSystemImage::mockDownloaded()
{
    Q_EMIT (updateDownloaded());
}

void MockSystemImage::mockFailed(const int &consecutiveFailureCount, const QString &lastReason)
{
    Q_EMIT (updateFailed(consecutiveFailureCount, lastReason));
}
