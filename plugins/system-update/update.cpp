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
    if (!error.isEmpty() && m_error != error) {
        m_error = error;
        Q_EMIT errorChanged();
    }
}

void Update::setUpdateAvailable(bool available)
{
    m_update = available;
}

void Update::setSystemUpdate(bool isSystem)
{
    if (m_systemUpdate != isSystem) {
        m_systemUpdate = isSystem;
        Q_EMIT systemUpdateChanged();
    }
}

void Update::setUpdateRequired(bool state)
{
    if (m_update != state) {
        m_update = state;
        Q_EMIT updateRequiredChanged();
    }
}

void Update::setUpdateState(bool state)
{
    if (m_update_state != state) {
        m_update_state = state;
        Q_EMIT updateStateChanged();
    }
}

void Update::setUpdateReady(bool ready)
{
    if (m_update_ready != ready) {
        m_update_ready = ready;
        Q_EMIT updateReadyChanged();
    }
}

void Update::setSelected(bool value)
{
    if (m_selected != value) {
        m_selected = value;
        Q_EMIT selectedChanged();
    }
}

void Update::setBinaryFilesize(int size)
{
    if (m_binary_filesize != size) {
        m_binary_filesize = size;
        Q_EMIT binaryFilesizeChanged();
    }
}

void Update::setIconUrl(QString icon)
{
    if (m_icon_url != icon) {
        m_icon_url = icon;
        Q_EMIT iconUrlChanged();
    }
}

void Update::setLastUpdateDate(const QString date)
{
    if (m_lastUpdateDate != date) {
        m_lastUpdateDate = date;
        Q_EMIT lastUpdateDateChanged();
    }
}

void Update::setClickUrl(const QString &url)
{
    m_click_url = url;
}

void Update::setDownloadProgress(int progress)
{
    if (m_download_progress != progress) {
        m_download_progress = progress;
        Q_EMIT downloadProgressChanged();
    }
}

void Update::setDownloadUrl(const QString &url)
{
    if (m_downloadUrl != url) {
        m_downloadUrl = url;
        Q_EMIT downloadUrlChanged();
    }
}

void Update::setClickToken(const QString &token)
{
    if (m_clickToken != token) {
        m_clickToken = token;
        Q_EMIT clickTokenChanged();
    }
}

bool Update::getIgnoreUpdates()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString value = environment.value("IGNORE_UPDATES", QString("NOT_IGNORE_UPDATES"));
    return value == "IGNORE_UPDATES";
}

}
