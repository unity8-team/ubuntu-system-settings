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
 * Ken VanDine <ken.vandine@canonical.com>
 *
*/

#include "connman.h"

/* A Wrapper class for OfonoConnMan (ConnectionManager)
 *
 * This class provides properties related to data connectivity (not voice)
 * via the modem
 */
ConnMan::ConnMan()
{
    m = new OfonoConnMan(OfonoModem::AutomaticSelect, QString(), NULL);
    QObject::connect(m, SIGNAL(roamingAllowedChanged(bool)), this, SLOT(onRoamingAllowedChanged(bool)));
    m_roam = m->roamingAllowed();

    qDebug() << "ROAMING ALLOWED: " << m_roam;

    QObject::connect(m, SIGNAL(poweredChanged(bool)), this, SLOT(onPoweredChanged(bool)));
    m_powered = m->powered();
    qDebug() << "POWERED: " << m_powered;
}

/* Contains whether data roaming is allowed.  In the off
 * setting, if the packet radio registration state
 * indicates that the modem is roaming, oFono will
 * automatically detach and no further connection
 * establishment will be possible.
 */
bool ConnMan::roamingAllowed() const
{
    return m_roam;
}

void ConnMan::setRoamingAllowed(const bool &st)
{
    m->setRoamingAllowed(st);
}

void ConnMan::onRoamingAllowedChanged(bool st)
{
    m_roam = st;
    emit roamingAllowedChanged();
    qDebug() << "ROAMING ALLOWED: " << m_roam;
}

/* Controls whether packet radio use is allowed. Setting
 * this value to off detaches the modem from the
 * Packet Domain network.
 */
bool ConnMan::powered() const
{
    return m_powered;
}

void ConnMan::setPowered(const bool &st)
{
    m->setPowered(st);
}

void ConnMan::onPoweredChanged(bool st)
{
    m_powered = st;
    emit poweredChanged();
    qDebug() << "POWERED: " << m_powered;
}
