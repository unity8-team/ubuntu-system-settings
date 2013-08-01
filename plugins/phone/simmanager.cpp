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
 * Ken VanDine <ken.vandine@canonical.com>
 *
*/

#include "simmanager.h"
#include "simservice.h"

SimManager::SimManager(QObject *parent) :
    QObject(parent)
{

    m_ofonoSimManager = new OfonoSimManager(OfonoModem::AutomaticSelect, QString(), this);

    m_present = m_ofonoSimManager->present();

    populateServiceNumbers(m_ofonoServiceNumbers);
    QObject::connect(m_ofonoSimManager,
        SIGNAL (serviceNumbersChanged (const OfonoServiceNumbers&)),
        this,
        SLOT (simServiceNumbersChanged (const OfonoServiceNumbers&)));

    QObject::connect(m_ofonoSimManager,
        SIGNAL (presenceChanged (bool)),
        this,
        SLOT (simPresenceChanged (bool)));

}

void SimManager::populateServiceNumbers (OfonoServiceNumbers sn)
{

    if (m_ofonoSimManager->modem()->isValid())
    {
        m_serviceNumbers.clear();
        if (sn.isEmpty())
            m_ofonoServiceNumbers = m_ofonoSimManager->serviceNumbers();
        else
            m_ofonoServiceNumbers = sn;

        QMapIterator<QString, QString> i(m_ofonoServiceNumbers);
        while (i.hasNext()) {
            i.next();
            m_serviceNumbers.append(new SimService(i.key(), i.value()));
        }
    }
}

bool SimManager::present() const
{
    return m_present;
}

QVariant SimManager::serviceNumbers()
{
    return QVariant::fromValue(m_serviceNumbers);
}

void SimManager::simPresenceChanged(bool ispresent)
{
    m_present = ispresent;
    emit presenceChanged(m_present);
}

void SimManager::simServiceNumbersChanged(OfonoServiceNumbers sn)
{
    populateServiceNumbers(sn);
    emit serviceNumbersChanged();
}

SimManager::~SimManager()
{
}
