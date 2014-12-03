/*
 * Copyright (C) 2014 - Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Diego Sarmentero <diego.sarmentero@canonical.com>
 */

#include <QDebug>
#include <QProcessEnvironment>

#include <ubuntu/download_manager/download_struct.h>
#include <ubuntu/download_manager/error.h>

#include "download_tracker.h"
#include "network/network.h"

namespace {
    const QString DOWNLOAD_COMMAND = "post-download-command";
    const QString APP_ID = "app_id";
    const QString PKCON_COMMAND = "pkcon";
}

namespace UpdatePlugin {

DownloadTracker::DownloadTracker(QObject *parent) :
    QObject(parent),
    m_clickToken(QString::null),
    m_downloadUrl(QString::null),
    m_download(nullptr),
    m_manager(nullptr),
    m_progress(0)
{
}

void DownloadTracker::setDownload(const QString& url)
{
    if (!url.isEmpty()) {
        m_downloadUrl = url;
        startService();
    }
}

void DownloadTracker::setClickToken(const QString& token)
{
    if (!token.isEmpty()) {
        m_clickToken = token;
        startService();
    }
}

void DownloadTracker::setPackageName(const QString& package)
{
    if (!package.isEmpty()) {
        m_packageName = package;
        startService();
    }
}

void DownloadTracker::startService()
{
    qDebug() <<  __PRETTY_FUNCTION__;

    if (!m_clickToken.isEmpty() && !m_downloadUrl.isEmpty() && !m_packageName.isEmpty()) {
        if (m_manager == nullptr) {
	    qDebug() << "Manager is null, creating new instance";
            m_manager = Manager::createSessionManager("", this);

            QObject::connect(m_manager, SIGNAL(downloadCreated(Download*)),
                             this, SLOT(bindDownload(Download*)));
        }
        QVariantMap vmap;
        QStringList args;
        QString command = getPkconCommand();
        args << command << "-p" << "install-local" << "$file";
        vmap[DOWNLOAD_COMMAND] = args;
        vmap[APP_ID] = m_packageName;
        StringMap map;
        map[X_CLICK_TOKEN] = m_clickToken;
        DownloadStruct dstruct = DownloadStruct(m_downloadUrl, vmap, map);
        m_manager->createDownload(dstruct);
    }
}

void DownloadTracker::bindDownload(Download* download)
{
    qDebug() <<  __PRETTY_FUNCTION__;
    m_download = download;
    connect(m_download, SIGNAL(finished(const QString &)), this,
            SIGNAL(onDownloadFinished(const QString &)));
    connect(m_download, SIGNAL(canceled(bool)), this,
            SIGNAL(onDownloadCanceled(bool)));
    connect(m_download, SIGNAL(paused(bool)), this,
            SIGNAL(paused(bool)));
    connect(m_download, SIGNAL(resumed(bool)), this,
            SIGNAL(resumed(bool)));
    connect(m_download, SIGNAL(started(bool)), this,
            SIGNAL(started(bool)));
    connect(m_download, SIGNAL(error(Error*)), this,
            SLOT(registerError(Error*)));
    connect(m_download, SIGNAL(progress(qulonglong, qulonglong)), this,
            SLOT(setProgress(qulonglong, qulonglong)));
    connect(m_download, SIGNAL(processing(const QString &)), this,
            SIGNAL(processing(const QString &)));

    m_download->start();
}

void DownloadTracker::registerError(Error* error)
{
    qDebug() <<  __PRETTY_FUNCTION__ << error->errorString();

    Q_EMIT errorFound(error->errorString());

    // we need to ensure that the resources are cleaned
    delete m_download;
    m_download = nullptr;
}

void DownloadTracker::onDownloadFinished(const QString& path)
{
    qDebug() <<  __PRETTY_FUNCTION__ << path;
    // once a download is finished we need to clean the resources
    delete m_download;
    m_download = nullptr;
    Q_EMIT finished(path);
}

void DownloadTracker::onDownloadCanceled(bool wasCanceled)
{
    if (wasCanceled) {
        delete m_download;
        m_download = nullptr;
    }
    Q_EMIT canceled(wasCanceled);
}

void DownloadTracker::pause()
{
    if (m_download != nullptr) {
        m_download->pause();
    }
}

void DownloadTracker::resume()
{
    if (m_download != nullptr) {
        m_download->resume();
    }
}

QString DownloadTracker::getPkconCommand()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("PKCON_COMMAND", QString(PKCON_COMMAND));
    return command;
}

void DownloadTracker::setProgress(qulonglong received, qulonglong total)
{
    qDebug() <<  __PRETTY_FUNCTION__ << received << total;
    if (total > 0) {
        qulonglong result = (received * 100);
        m_progress = static_cast<int>(result / total);
        emit progressChanged();
    }
}

}
