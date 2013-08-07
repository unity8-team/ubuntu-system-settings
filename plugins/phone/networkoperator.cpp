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
    qDebug() << "operatorId: " << operatorId;
    m_ofonoNetworkOperator = new OfonoNetworkOperator(operatorId);

    m_name = m_ofonoNetworkOperator->name();
    qDebug() << "NAME: " << m_name;
    m_status = m_ofonoNetworkOperator->status();
    qDebug() << "STATUS: " << m_status;
    QObject::connect(m_ofonoNetworkOperator,
        SIGNAL (nameChanged (const QString&)),
        this,
        SLOT (operatorNameChanged(const QString&)));
    QObject::connect(m_ofonoNetworkOperator,
        SIGNAL (statusChanged (const QString&)),
        this,
        SLOT (operatorStatusChanged(const QString&)));
    QObject::connect(m_ofonoNetworkOperator,
        SIGNAL (registerComplete (bool)),
        this,
        SLOT (operatorRegisterComplete(bool)));
}

/*
NetworkOperator::NetworkOperator(const NetworkOperator& op)
    : QObject(op.parent())
{
    new OfonoNetworkOperator(op);
}
*/

NetworkOperator::CellDataTechnology NetworkOperator::technologyToInt(const QString &technology)
{
    if (technology == QString(QStringLiteral("gprs")))
        return NetworkOperator::GprsDataTechnology;
    else if (technology == QString(QStringLiteral("edge")))
        return NetworkOperator::EdgeDataTechnology;
    else if (technology == QString(QStringLiteral("umts")))
        return NetworkOperator::UmtsDataTechnology;
    else if (technology == QString(QStringLiteral("hspa")))
        return NetworkOperator::HspaDataTechnology;

    return NetworkOperator::UnknownDataTechnology;
}

QString NetworkOperator::name() const
{
    return m_name;
}

void NetworkOperator::operatorNameChanged(const QString &name)
{
    m_name = name;
    qDebug() << "NAME: " << m_name;
    emit nameChanged(m_name);
}

QString NetworkOperator::status() const
{
    return m_status;
}

void NetworkOperator::operatorStatusChanged(const QString &status)
{
    m_status = status;
    qDebug() << "STATUS: " << m_status;
    emit statusChanged(m_status);
}

void NetworkOperator::registerOp()
{
    qDebug() << "Registering on " << m_name;
    m_ofonoNetworkOperator->registerOp();
}

void NetworkOperator::operatorRegisterComplete(bool success)
{
    if (success)
    {
        qDebug() << "operatorRegisterComplete: SUCCESS";
    }
    else
    {
        qDebug() << "operatorRegisterComplete: FAILED " << m_ofonoNetworkOperator->errorMessage();
    }
    emit registerComplete(success, m_ofonoNetworkOperator->errorMessage());
}

NetworkOperator::~NetworkOperator()
{
}
