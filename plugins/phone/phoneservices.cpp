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
 * Authors: Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#include <ofonosimmanager.h>
#include <QtDebug>

#include "phoneservices.h"

PhoneServices::PhoneServices(QObject *parent) :
    QObject(parent)
{

}

QMap<QString, QVariant> PhoneServices::serviceNumbers()
{
    if (m_serviceNumbers.isEmpty())
    {
        OfonoSimManager *sim = new OfonoSimManager(OfonoModem::AutomaticSelect, QString(), NULL);

        if (sim->modem()->isValid())
        {
            m_serviceNumbers.clear();
            QMap<QString, QString> sn = sim->serviceNumbers();
            QMapIterator<QString, QString> i(sn);
            while (i.hasNext()) {
                i.next();
                m_serviceNumbers[i.key()] = i.value();
                qDebug() << i.key() << ": " << i.value() << endl;
            }

        }
    }
    return m_serviceNumbers;
}

PhoneServices::~PhoneServices() {
}
