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


void MockSystemImage::factoryReset() {
}

void MockSystemImage::productionReset() {
}

void MockSystemImage::checkForUpdate() {
}

void MockSystemImage::downloadUpdate() {
}

void MockSystemImage::forceAllowGSMDownload() {
}

void MockSystemImage::applyUpdate() {
}

QString MockSystemImage::cancelUpdate() {
}

QString MockSystemImage::pauseDownload() {
}

bool MockSystemImage::checkTarget() const
{
    return m_targetBuildNumber > m_currentBuildNumber;
}

bool MockSystemImage::checkingForUpdates() const
{
    return m_checkingForUpdates;
}

QString MockSystemImage::deviceName() const
{
    return m_deviceName;
}

QString MockSystemImage::channelName() const
{
    return m_channelName;
}

QDateTime MockSystemImage::lastUpdateDate() const
{
    return m_lastUpdateDate;
}

QDateTime MockSystemImage::lastCheckDate() const
{
    return m_lastCheckDate;
}

bool MockSystemImage::updateAvailable()
{
    return m_updateAvailable;
}

bool MockSystemImage::downloading()
{
    return m_downloading;
}

int MockSystemImage::updateSize()
{
    return m_updateSize;
}

QString MockSystemImage::errorReason()
{
    return m_errorReason;
}

QString MockSystemImage::versionTag()
{
    QString val = m_detailedVersion.value("tag").toString();
    return val.isEmpty() ? "" : val;
}

int MockSystemImage::currentBuildNumber() const
{
    return m_currentBuildNumber;
}

QString MockSystemImage::currentUbuntuBuildNumber() const
{
    QString val = m_detailedVersion.value("ubuntu").toString();
    return val.isEmpty() ? "Unavailable" : val;
}

QString MockSystemImage::currentDeviceBuildNumber() const
{
    QString val = m_detailedVersion.value("device").toString();
    return val.isEmpty() ? "Unavailable" : val;
}

QString MockSystemImage::currentCustomBuildNumber() const
{
    QString val = m_detailedVersion.value("custom").toString();
    return val.isEmpty() ? "Unavailable" : val;
}

int MockSystemImage::targetBuildNumber() const
{
    return m_targetBuildNumber;
}

QVariantMap MockSystemImage::detailedVersionDetails() const
{
    return m_detailedVersion;
}

int MockSystemImage::downloadMode()
{
    return m_downloadMode;
}

void MockSystemImage::setDownloadMode(const int &downloadMode) {
    if (m_downloadMode == downloadMode) {
        return;
    }

    if (downloadMode < 0 || downloadMode > 2) {
        return;
    }

    m_downloadMode = downloadMode;
}

void MockSystemImage::mockProgress(const int &percentage, const double &eta)
{
    Q_EMIT updateProgress(percentage, eta);
}

void MockSystemImage::mockAvailableStatus(const bool isAvailable,
                                          const bool downloading,
                                          const QString availableVersion,
                                          const int updateSize,
                                          const QString lastUpdateDate,
                                          const QString errorReason)
{
    Q_EMIT updateAvailableStatus(isAvailable,
                                 downloading,
                                 availableVersion,
                                 updateSize,
                                 lastUpdateDate,
                                 errorReason);
}

void MockSystemImage::mockPaused(const int &percentage)
{
    Q_EMIT updatePaused(percentage);
}

void MockSystemImage::mockStarted()
{
    Q_EMIT downloadStarted();
}

void MockSystemImage::mockDownloaded()
{
    Q_EMIT updateDownloaded();
}

void MockSystemImage::mockFailed(const int &consecutiveFailureCount, const QString &lastReason)
{
    Q_EMIT updateFailed(consecutiveFailureCount, lastReason);
}

void MockSystemImage::mockTargetBuildNumber(const uint &target)
{
    // m_targetBuildNumber = target;
    Q_EMIT targetBuildNumberChanged();
}

void MockSystemImage::mockCurrentBuildNumber(const uint &current)
{
    // m_currentBuildNumber = current;
    Q_EMIT currentBuildNumberChanged();
}
