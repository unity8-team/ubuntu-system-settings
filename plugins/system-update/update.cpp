/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Authors:
 * Diego Sarmentero <diego.sarmentero@canonical.com>
 *
*/

#include <QProcessEnvironment>
#include <QStringList>

#include <apt-pkg/debversion.h>

#include "update.h"

namespace UpdatePlugin {

Update::Update(QObject *parent) :
    QObject(parent)
{
}

Update::~Update()
{
}

void Update::initializeApplication(QString packagename, QString title,
                                   QString version)
{
    m_packagename = packagename;
    m_title = title;
    m_local_version = version;

    Q_EMIT packageNameChanged();
    Q_EMIT titleChanged();
    Q_EMIT localVersionChanged();
}

void Update::setRemoteVersion(QString& version)
{
    m_remote_version = version;
    if (!getIgnoreUpdates()) {
        int result = debVS.CmpVersion(m_local_version.toUtf8().data(),
                                      m_remote_version.toUtf8().data());

        m_update = result < 0;
    } else {
        m_update = false;
    }
}

void Update::setError(QString error)
{
    m_error = error;
    if (!m_error.isEmpty()) {
        Q_EMIT errorChanged();
    }
}

void Update::setSystemUpdate(bool isSystem)
{
    m_systemUpdate = isSystem;
    Q_EMIT systemUpdateChanged();
}

void Update::setUpdateRequired(bool state)
{
    m_update = state;
    Q_EMIT updateRequiredChanged();
}

void Update::setUpdateState(bool state)
{
    m_update_state = state;
    Q_EMIT updateStateChanged();
}

void Update::setUpdateReady(bool ready)
{
    m_update_ready = ready;
    Q_EMIT updateReadyChanged();
}

void Update::setSelected(bool value)
{
    m_selected = value;
    Q_EMIT selectedChanged();
}

void Update::setBinaryFilesize(int size)
{
    m_binary_filesize = size;
    Q_EMIT binaryFilesizeChanged();
}

void Update::setIconUrl(QString icon) {
    m_icon_url = icon;
    Q_EMIT iconUrlChanged();
}

void Update::setLastUpdateDate(const QString date)
{
    m_lastUpdateDate = date;
    Q_EMIT lastUpdateDateChanged();
}

void Update::setDownloadProgress(int progress)
{
    m_download_progress = progress;
    Q_EMIT downloadProgressChanged();
}

void Update::setDownloadUrl(const QString &url) {
    m_downloadUrl = url;
    Q_EMIT downloadUrlChanged();
}

bool Update::getIgnoreUpdates()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString value = environment.value("IGNORE_UPDATES", QString("NOT_IGNORE_UPDATES"));
    return value == "IGNORE_UPDATES";
}

}
