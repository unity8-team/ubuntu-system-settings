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

MockSingleDownload::MockSingleDownload(QObject *parent)
{

}

void MockSingleDownload::start()
{

}

void MockSingleDownload::pause()
{

}

void MockSingleDownload::resume()
{

}

void MockSingleDownload::cancel()
{

}

void MockSingleDownload::download(QString url)
{

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
    return QString();
}

bool MockSingleDownload::allowMobileDownload() const
{

}

int MockSingleDownload::progress() const
{
    return 0;
}

bool MockSingleDownload::downloading() const
{
    return false;
}

bool MockSingleDownload::downloadInProgress() const
{
    return false;
}

bool MockSingleDownload::autoStart() const
{
    return false;
}

QString MockSingleDownload::downloadId() const
{

}

QVariantMap MockSingleDownload::headers() const
{

}

MockMetadata*MockSingleDownload:: metadata() const
{

}

void MockSingleDownload::setAllowMobileDownload(bool value)
{

}

void MockSingleDownload::setHeaders(QVariantMap headers)
{

}

void MockSingleDownload::setMetadata(MockMetadata* metadata)
{

}

void MockSingleDownload::setAutoStart(bool value) {

}

void MockSingleDownload::onFinished(const QString& path)
{

}

void MockSingleDownload::onProgress(qulonglong received, qulonglong total)
{

}

void MockSingleDownload::onPaused(bool wasPaused)
{

}

void MockSingleDownload::onResumed(bool wasResumed)
{

}

void MockSingleDownload::onStarted(bool wasStarted)
{

}

void MockSingleDownload::onCanceled(bool wasCanceled)
{

}
