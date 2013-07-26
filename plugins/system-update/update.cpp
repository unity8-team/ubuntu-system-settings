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
#include <QDebug>
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

Update::Update(QObject *parent) :
    QObject(parent),
    m_updateAvailable(-1), // no status available
    m_sessionBusConnection (QDBusConnection::sessionBus()),
    m_SystemServiceIface ("com.canonical.SystemImage",
                         "/Service",
                         "com.canonical.SystemImage",
                         m_sessionBusConnection)
{

    // TODO: check if we get an error and maybe trigger a signal to update the UI (or retry)
    // check if an update is available
    connect(&m_SystemServiceIface, SIGNAL(UpdateAvailableStatus(bool)),
            this, SLOT(slotUpdateAvailableStatus(bool)));
    m_SystemServiceIface.call("CheckForUpdate");

    // get current OS version
    QDBusReply<int> reply = m_SystemServiceIface.call("BuildNumber");
    if (reply.isValid())
        m_OSVersion = QString::number(reply.value());
    else
        m_OSVersion = "Unknown";

    // signals to forward directly to QML
    connect(&m_SystemServiceIface, SIGNAL(ReadyToReboot()),
            this, SIGNAL(readyToReboot()));
    connect(&m_SystemServiceIface, SIGNAL(UpdateFailed()),
            this, SIGNAL(updateFailed()));
    connect(&m_SystemServiceIface, SIGNAL(Canceled()),
            this, SIGNAL(updateCanceled()));
}

Update::~Update() {
}

void Update::TriggerUpdate() {
    m_SystemServiceIface.call("GetUpdate");
}

void Update::CancelUpdate() {
    m_SystemServiceIface.call("Cancel");
}

void Update::Reboot() {
    m_SystemServiceIface.call("Reboot");
}

QString Update::OSVersion()
{
    return m_OSVersion;
}

QString Update::UpdateVersion()
{
    return m_updateVersion;
}

QString Update::UpdateSize()
{
    return m_updateSize;
}

QString Update::UpdateDescriptions()
{
    return m_updateDescriptions;
}

bool Update::slotUpdateAvailableStatus(bool pendingUpdate)
{
    m_updateAvailable = int(pendingUpdate);
    if (pendingUpdate)
        m_getUpdateInfos();
    Q_EMIT updateAvailableChanged();
    return pendingUpdate;
}

int Update::UpdateAvailable()
{
    return m_updateAvailable;
}

void Update::m_getUpdateInfos()
{
    QDBusReply<int> reply = m_SystemServiceIface.call("GetUpdateVersion");
    if (reply.isValid())
        m_updateVersion = QString::number(reply.value());
    else
        m_updateVersion = "Unknown";

    QDBusReply<qint64> reply2 = m_SystemServiceIface.call("GetUpdateSize");
    if (reply2.isValid())
        m_updateSize = QString("%1 Mb").arg(QString::number(reply2.value()/1024.0));
    else
        m_updateSize = "Unknown";

    // TODO: descriptions (array of dict)

}
