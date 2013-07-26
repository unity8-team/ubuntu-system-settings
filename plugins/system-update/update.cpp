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
    m_SystemServiceIface.call("CheckForUpdate");

    connect(&m_SystemServiceIface, SIGNAL(UpdateAvailableStatus(bool)),
            this, SLOT(slotUpdateAvailableStatus(bool)));

}

Update::~Update() {
}


QString Update::versionOS()
{
    if (m_versionOS.isEmpty() || m_versionOS.isNull())
        m_versionOS = QString("%1 %2").arg(m_objectPath).arg(QString("as user id"));

    return m_versionOS;
}


bool Update::slotUpdateAvailableStatus(bool pendingUpdate)
{
    m_updateAvailable = int(pendingUpdate);
    Q_EMIT updateAvailableChanged();
    return pendingUpdate;
}

int Update::getUpdateAvailable()
{
    return m_updateAvailable;
}
