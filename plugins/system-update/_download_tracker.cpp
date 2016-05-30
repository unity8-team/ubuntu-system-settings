/*
 * Copyright (C) 2014-2015 - Canonical Ltd.
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

#include <QProcessEnvironment>
#include <ubuntu/download_manager/download_struct.h>
#include <ubuntu/download_manager/error.h>

#include "download_tracker.h"
#include "network.h"

namespace {
    const QString DOWNLOAD_COMMAND = "post-download-command";
    const QString PACKAGE_NAME = "package-name";
    const QString TITLE = "title";
    const QString SHOW_IN_INDICATOR = "indicator-shown";
    const QString PKCON_COMMAND = "pkcon";
    const QString DOWNLOAD_MANAGER_SHA512 = "sha512";
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

void DownloadTracker::setTitle(const QString& title)
{
    if (!title.isEmpty()) {
        m_title = title;
        startService();
    }
}

void DownloadTracker::startService()
{
    if (!m_clickToken.isEmpty() && !m_downloadUrl.isEmpty() && !m_packageName.isEmpty() && !m_title.isEmpty()) {
        if (m_manager == nullptr) {
            m_manager = Manager::createSessionManager("", this);

            if (!connect(m_manager, &Manager::downloadCreated,
                this, &DownloadTracker::bindDownload)) {
                qWarning() << "Could not connect to Manager::downloadCreated!";
	    }
        }
        QVariantMap vmap;
        QStringList args;
        QString command = getPkconCommand();
        args << command << "-p" << "install-local" << "$file";
        vmap[DOWNLOAD_COMMAND] = args;
        vmap[PACKAGE_NAME] = m_packageName;
        vmap[TITLE] = m_title;
        vmap[SHOW_IN_INDICATOR] = m_show_in_indicator;
        StringMap map;
        map[X_CLICK_TOKEN] = m_clickToken;
        DownloadStruct dstruct(m_downloadUrl, m_download_sha512, DOWNLOAD_MANAGER_SHA512, vmap, map);
        m_manager->createDownload(dstruct);
    }
}

void DownloadTracker::bindDownload(Download* download)
{
    m_download = download;
    if (!connect(m_download, &Download::finished,
            this, &DownloadTracker::onDownloadFinished)) {
        qWarning() << "Could not connect to Download::finished";
    }
    if (!connect(m_download, &Download::canceled,
            this, &DownloadTracker::onDownloadCanceled)) {
        qWarning() << "Could not connect to Download::canceled";
    }
    if (!connect(m_download, &Download::paused,
            this, &DownloadTracker::paused)) {
        qWarning() << "Could not connect to Download::paused";
    }
    if (!connect(m_download, &Download::resumed,
            this, &DownloadTracker::resumed)) {
        qWarning() << "Could not connect to Download::resumed";
    }
    if (!connect(m_download, &Download::started,
            this, &DownloadTracker::started)) {
        qWarning() << "Could not connect to Download::started";
    }
    if (!connect(m_download, static_cast<void(Download::*)(Error*)>(&Download::error),
            this, &DownloadTracker::registerError)) {
        qWarning() << "Could not connect to Download::error";
    }

    if (!connect(m_download, static_cast<void(Download::*)(qulonglong, qulonglong)>(&Download::progress),
            this, &DownloadTracker::setProgress)) {
        qWarning() << "Could not connect to Download::progress";
    }

    if (!connect(m_download, &Download::processing,
            this, &DownloadTracker::processing)) {
        qWarning() << "Could not connect to Download::processing";
    }

    m_download->start();
}

void DownloadTracker::registerError(Error* error)
{
    Q_EMIT errorFound(error->errorString());

    // we need to ensure that the resources are cleaned
    m_download->deleteLater();
    m_download = nullptr;
}

void DownloadTracker::onDownloadFinished(const QString& path)
{
    // once a download is finished we need to clean the resources
    m_download->deleteLater();
    m_download = nullptr;
    Q_EMIT finished(path);
}

void DownloadTracker::onDownloadCanceled(bool wasCanceled)
{
    if (wasCanceled) {
        m_download->deleteLater();
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
    if (total > 0) {
        qulonglong result = (received * 100);
        m_progress = static_cast<int>(result / total);
        emit progressChanged();
    }
}

}