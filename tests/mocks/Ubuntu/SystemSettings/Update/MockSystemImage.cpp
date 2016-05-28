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

#include "MockSystemImage.h"

MockSystemImage::MockSystemImage(QObject *parent) :
    QObject(parent)
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
    return 0;
}

void MockSystemImage::setDownloadMode(const int &downloadMode)
{
}
