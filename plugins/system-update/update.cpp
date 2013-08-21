/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Didier Roche <didier.roche@canonical.com>
 *
*/

#include "update.h"
#include "../../src/i18n.h"
#include <QDebug>
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

using namespace SystemSettings;

Update::Update(QObject *parent) :
    QObject(parent),
    m_infoMessage(""),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_SystemServiceIface ("com.canonical.SystemImage",
                         "/Service",
                         "com.canonical.SystemImage",
                         m_systemBusConnection)
{

    // TODO: remove? a slot to filter the function
    /*connect(&m_SystemServiceIface, SIGNAL(UpdateAvailableStatus(bool updateS)),
            this, SLOT(slotUpdateAvailableStatus(bool)));*/

    // signals to forward directly to QML
    connect(&m_SystemServiceIface, SIGNAL(UpdateAvailableStatus(bool)),
            this, SIGNAL(updateAvailableStatus(bool)));
    connect(&m_SystemServiceIface, SIGNAL(UpdateProgress(int, int)),
                this, SIGNAL(updateProgress(int, int)));
    connect(&m_SystemServiceIface, SIGNAL(UpdatePaused(int)),
                this, SIGNAL(updatePaused(int)));
    connect(&m_SystemServiceIface, SIGNAL(UpdateDownloaded()),
                this, SIGNAL(updateDownloaded()));
    connect(&m_SystemServiceIface, SIGNAL(UpdateFailed(int, QString)),
                this, SIGNAL(updateDownloaded(int, QString)));

    this->CheckForUpdate();

}

Update::~Update() {
}

void Update::CheckForUpdate() {
    m_SystemServiceIface.call("CheckForUpdate");
}

void Update::DownloadUpdate() {
    m_SystemServiceIface.call("DownloadUpdate");
}

QString Update::ApplyUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("ApplyUpdate");
    if (reply.isValid())
        return reply.value();
    return _("Can't apply the current update (can't contact service)");
}

QString Update::CancelUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("CancelUpdate");
    if (reply.isValid())
        return reply.value();
    return _("Can't cancel current request (can't contact service)");
}

QString Update::PauseUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("PauseUpdate");
    if (reply.isValid())
        return reply.value();
    return _("Can't pause current request (can't contact service)");
}

QString Update::InfoMessage() {
    return m_infoMessage;
}

void Update::SetInfoMessage(QString infoMessage) {
    m_infoMessage = infoMessage;
    Q_EMIT infoMessageChanged();
}

QString Update::TranslateFromBackend(QString msg) {
    // TODO: load in the backend context .mo file
    return msg;
}

/*
bool Update::slotUpdateAvailableStatus(bool pendingUpdate)
{
    m_updateAvailable = int(pendingUpdate);
    if (pendingUpdate)
        m_getUpdateInfos();
    Q_EMIT updateAvailableChanged();
    return pendingUpdate;
}


    QDBusReply<qint64> reply2 = m_SystemServiceIface.call("GetUpdateSize");
    if (reply2.isValid())
        m_updateSize = QString("%1 Mb").arg(QString::number(reply2.value()/1024.0/1024.0));
    else
        m_updateSize = "Unknown";

    // TODO: descriptions (array of dict)

}
*/
