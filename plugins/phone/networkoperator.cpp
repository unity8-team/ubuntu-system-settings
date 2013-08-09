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

#include "networkoperator.h"

NetworkOperator::NetworkOperator(const QString& operatorId, QObject *parent) :
    QObject(parent)
{
    m_ofonoNetworkOperator = new OfonoNetworkOperator(operatorId);

    QObject::connect(m_ofonoNetworkOperator,
        SIGNAL (nameChanged (const QString&)),
        this,
        SLOT (operatorNameChanged(const QString&)));
    m_name = m_ofonoNetworkOperator->name();


    QObject::connect(m_ofonoNetworkOperator,
        SIGNAL (statusChanged (const QString&)),
        this,
        SLOT (operatorStatusChanged(const QString&)));
        m_status = m_ofonoNetworkOperator->status();

    QObject::connect(m_ofonoNetworkOperator,
        SIGNAL (registerComplete (bool)),
        this,
        SLOT (operatorRegisterComplete(bool)));
}

/* Contains the name of the operator */
QString NetworkOperator::name() const
{
    return m_name;
}

void NetworkOperator::operatorNameChanged(const QString &name)
{
    m_name = name;
    emit nameChanged(m_name);
}

/* Contains the status of the operator */
QString NetworkOperator::status() const
{
    return m_status;
}

void NetworkOperator::operatorStatusChanged(const QString &status)
{
    m_status = status;
    emit statusChanged(m_status);
}

/* Registers on the operator's network */
void NetworkOperator::registerOp()
{
    m_ofonoNetworkOperator->registerOp();
}

void NetworkOperator::operatorRegisterComplete(bool success)
{
    emit registerComplete(success, m_ofonoNetworkOperator->errorMessage());
}

NetworkOperator::~NetworkOperator()
{
}
