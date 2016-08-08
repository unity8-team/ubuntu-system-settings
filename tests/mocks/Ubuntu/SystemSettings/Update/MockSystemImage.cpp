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

void MockSystemImage::checkForUpdate()
{
    m_called << "checkForUpdate";
}

void MockSystemImage::downloadUpdate()
{
    m_called << "downloadUpdate";
}

void MockSystemImage::forceAllowGSMDownload()
{
    m_called << "forceAllowGSMDownload";
}

void MockSystemImage::applyUpdate()
{
    m_called << "applyUpdate";
}

QString MockSystemImage::cancelUpdate()
{
    return QString();
}

QString MockSystemImage::pauseDownload()
{
    m_called << "pauseDownload";
    return QString();
}

bool MockSystemImage::checkTarget() const
{
    return m_targetBuildNumber > m_currentBuildNumber;
}

bool MockSystemImage::checkingForUpdates() const
{
    return m_checkingForUpdates;
}

int MockSystemImage::currentBuildNumber() const
{
    return m_currentBuildNumber;
}

int MockSystemImage::targetBuildNumber() const
{
    return m_targetBuildNumber;
}

void MockSystemImage::mockTargetBuildNumber(const uint &target)
{
    m_targetBuildNumber = target;
    Q_EMIT targetBuildNumberChanged();
}

void MockSystemImage::mockCurrentBuildNumber(const uint &current)
{
    m_currentBuildNumber = current;
    Q_EMIT currentBuildNumberChanged();
}

void MockSystemImage::mockUpdateFailed(const int &consecutiveFailureCount,
                                       const QString &lastReason)
{
    Q_EMIT updateFailed(consecutiveFailureCount, lastReason);
}

void MockSystemImage::mockVersionTag(const QString &tag)
{
    m_versionTag = tag;
    Q_EMIT versionTagChanged();
}

QString MockSystemImage::versionTag() const
{
    return m_versionTag;
}

bool MockSystemImage::called(const QString &functionName)
{
    return m_called.contains(functionName);
}

void MockSystemImage::reset()
{
    m_called.clear();
}

int MockSystemImage::downloadMode()
{
    return m_downloadMode;
}

void MockSystemImage::setDownloadMode(const int &downloadMode)
{
    m_downloadMode = downloadMode;
    Q_EMIT downloadModeChanged();
}
