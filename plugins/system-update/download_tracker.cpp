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

#include "download_tracker.h"
#include "network/network.h"
#include <ubuntu/download_manager/download_struct.h>
#include <ubuntu/download_manager/error.h>
#include <QProcessEnvironment>

#define DOWNLOAD_COMMAND "post-download-command"
#define APP_ID "app_id"
#define PKCON_COMMAND "pkcon"
#define DOWNLOAD_MANAGER_SHA512 "sha512"

namespace UpdatePlugin {

DownloadTracker::DownloadTracker(QObject *parent) :
    QObject(parent),
    m_clickToken(""),
    m_downloadUrl(""),
    m_download(nullptr),
    m_manager(nullptr),
    m_progress(0)
{
}

void DownloadTracker::setDownload(const QString& url)
{
    if (url != "") {
        m_downloadUrl = url;
        startService();
    }
}

void DownloadTracker::setClickToken(const QString& token)
{
    if (token != "") {
        m_clickToken = token;
        startService();
    }
}

void DownloadTracker::setPackageName(const QString& package)
{
    if (package != "") {
        m_packageName = package;
        startService();
    }
}

void DownloadTracker::startService()
{
    if (!m_clickToken.isEmpty() && !m_downloadUrl.isEmpty() && !m_packageName.isEmpty()) {
        if (m_manager == nullptr) {
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
        DownloadStruct dstruct = DownloadStruct(m_downloadUrl, m_download_sha512, DOWNLOAD_MANAGER_SHA512, vmap, map);
        m_manager->createDownload(dstruct);
    }
}

void DownloadTracker::bindDownload(Download* download)
{
    m_download = download;
    connect(m_download, SIGNAL(finished(const QString &)), this,
            SIGNAL(finished(const QString &)));
    connect(m_download, SIGNAL(error(Error*)), this,
            SLOT(registerError(Error*)));
    connect(m_download, SIGNAL(progress(qulonglong, qulonglong)), this,
            SLOT(setProgress(qulonglong, qulonglong)));

    m_download->start();
}

void DownloadTracker::registerError(Error* error)
{
    Q_EMIT errorFound(error->errorString());
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
