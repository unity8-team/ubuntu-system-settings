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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
#include "radiosettings.h"

/* A Wrapper class for OfonoRadioSettings
 *
 * This class provides properties and functions related to
 * radio settings.
 */
RadioSettings::RadioSettings(QObject *parent) :
    QObject(parent)
{
    m_ofonoRadioSettings = new OfonoRadioSettings(OfonoModem::AutomaticSelect, QString(), this);

    qCritical() << "RadioSettings: ctor";

    QObject::connect(m_ofonoRadioSettings,
        SIGNAL (technologyPreferenceChanged (const QString&)),
        this,
        SLOT (operatorPreferedTechnologyChanged(const QString&)));
    m_preferedTechnology = m_ofonoRadioSettings->technologyPreference();
    qCritical() << "RadioSettings: " << m_preferedTechnology;
    qCritical() << "RadioSettings: m_if->path() " << m_ofonoRadioSettings;
}

/* Contains the name of the operator currently register */
QString RadioSettings::preferedTechnology() const
{
    return m_preferedTechnology;
}

void RadioSettings::operatorPreferedTechnologyChanged(const QString &preferedTechnology)
{
    qCritical() << "operatorPreferedTechnologyChanged: " << preferedTechnology;
    if (preferedTechnology != m_preferedTechnology)
    {
        m_preferedTechnology = preferedTechnology;
        Q_EMIT preferedTechnologyChanged();
        qCritical() << "RadioSettings: operatorPreferedTechnologyChanged, new val: " << preferedTechnology;
    } else {
        qCritical() << "RadioSettings: operatorPreferedTechnologyChanged changed, no change";
    }
}

void RadioSettings::setPreferedTechnology(QString &preferedTechnology)
{
    m_ofonoRadioSettings->setTechnologyPreference(preferedTechnology);
}

RadioSettings::~RadioSettings()
{
}
