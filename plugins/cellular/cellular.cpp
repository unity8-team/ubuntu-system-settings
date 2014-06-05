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
#include <QObject>
#include <QtDebug>

#include "cellular.h"

Cellular::Cellular(QObject *parent) :
    QObject(parent)
{
    qCritical() << "Cellular: ctor";
}

QString Cellular::getTechnologyPreference()
{
    return QString("any");
}

QString Cellular::technologyPreference()
{
    if (m_currentTechnologyPreference.isEmpty() || m_currentTechnologyPreference.isNull())
        m_currentTechnologyPreference = getTechnologyPreference();

     return m_currentTechnologyPreference;
}

void Cellular::setTechnologyPreference(QString &pref)
{
    m_currentTechnologyPreference = pref;
    Q_EMIT technologyPreferenceChanged();
}
