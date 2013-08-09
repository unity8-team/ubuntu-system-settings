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
#include "networkregistration.h"

NetworkRegistration::NetworkRegistration(QObject *parent) :
    QObject(parent)
{
    m_ofonoNetworkRegistration = new OfonoNetworkRegistration(OfonoModem::AutomaticSelect, QString(), this);

    QObject::connect(m_ofonoNetworkRegistration,
        SIGNAL (nameChanged (const QString&)),
        this,
        SLOT (operatorNameChanged(const QString&)));
    QObject::connect(m_ofonoNetworkRegistration,
        SIGNAL (statusChanged (const QString&)),
        this,
        SLOT (operatorStatusChanged(const QString&)));
    QObject::connect(m_ofonoNetworkRegistration,
        SIGNAL (modeChanged (const QString&)),
        this,
        SLOT (operatorModeChanged(const QString&)));
    QObject::connect(m_ofonoNetworkRegistration,
        SIGNAL (technologyChanged (const QString&)),
        this,
        SLOT (operatorTechnologyChanged(const QString&)));
    QObject::connect(m_ofonoNetworkRegistration,
        SIGNAL (getOperatorsComplete (bool, const QStringList&)),
        this,
        SLOT (operatorsUpdated(bool, const QStringList&)));
    QObject::connect(m_ofonoNetworkRegistration,
        SIGNAL (scanComplete (bool, const QStringList&)),
        this,
        SLOT (operatorsUpdated(bool, const QStringList&)));

    m_name = m_ofonoNetworkRegistration->name();
    qDebug() << "NAME: " << m_name;
    m_status = m_ofonoNetworkRegistration->status();
    qDebug() << "STATUS: " << m_status;
    m_mode = m_ofonoNetworkRegistration->mode();
    qDebug() << "MODE: " << m_mode;
    m_technology = technologyToInt(m_ofonoNetworkRegistration->technology());
    qDebug() << "TECHNOLOGY: " << m_technology;
    m_ofonoNetworkRegistration->getOperators();
}


QVariant NetworkRegistration::operators() const
{
    return QVariant::fromValue(m_operators);
}

void NetworkRegistration::populateOperators (QStringList oplist)
{

    m_operators.clear();
    foreach(QString i, oplist)
    {
        m_operators.append(new NetworkOperator(i));
    }

}

void NetworkRegistration::operatorsUpdated(bool success, const QStringList &oplist)
{
    this->setScanning(false);
    if (success)
    {
        populateOperators(oplist);
        qDebug() << "operatorsUpdated: " << oplist;
        emit operatorsChanged();
    }
}

void NetworkRegistration::scan()
{
    this->setScanning(true);
    m_ofonoNetworkRegistration->scan();
}

void NetworkRegistration::getOperators()
{
    m_ofonoNetworkRegistration->getOperators();
}

NetworkRegistration::CellDataTechnology NetworkRegistration::technologyToInt(const QString &technology)
{
    if (technology == QString(QStringLiteral("gprs")))
        return NetworkRegistration::GprsDataTechnology;
    else if (technology == QString(QStringLiteral("edge")))
        return NetworkRegistration::EdgeDataTechnology;
    else if (technology == QString(QStringLiteral("umts")))
        return NetworkRegistration::UmtsDataTechnology;
    else if (technology == QString(QStringLiteral("hspa")))
        return NetworkRegistration::HspaDataTechnology;

    return NetworkRegistration::UnknownDataTechnology;
}

QString NetworkRegistration::name() const
{
    return m_name;
}

void NetworkRegistration::operatorNameChanged(const QString &name)
{
    m_name = name;
    qDebug() << "NAME: " << m_name;
    emit nameChanged(m_name);
}

QString NetworkRegistration::status() const
{
    return m_status;
}

void NetworkRegistration::operatorStatusChanged(const QString &status)
{
    m_status = status;
    qDebug() << "STATUS: " << m_status;
    emit statusChanged(m_status);
}

/* The network registration mode, possible values include: "auto",
 * "auto-only", and "manual".  The mode changes from "auto" to
 * "manual" when registerOp is called on an operator.
 */
QString NetworkRegistration::mode() const
{
    return m_mode;
}

void NetworkRegistration::operatorModeChanged(const QString &mode)
{
    m_mode = mode;
    qDebug() << "MODE: " << m_mode;
    emit modeChanged(m_mode);
}

NetworkRegistration::CellDataTechnology NetworkRegistration::technology() const
{
    return m_technology;
}

void NetworkRegistration::operatorTechnologyChanged(const QString &technology)
{
    m_technology = technologyToInt(technology);
    qDebug() << "TECHNOLOGY: " << m_technology;
    emit technologyChanged(m_technology);
}

bool NetworkRegistration::scanning() const
{
    return m_scanning;
}

void NetworkRegistration::setScanning(bool scanning)
{
    if (scanning != m_scanning)
    {
        m_scanning = scanning;
        emit scanningChanged();
    }
}

NetworkRegistration::~NetworkRegistration()
{
}
