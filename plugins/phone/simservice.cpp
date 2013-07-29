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

#include "simservice.h"

SimService::SimService(QObject *parent)
    : QObject(parent)
{
}

SimService::SimService(const QString &name, const QString &value, QObject *parent)
    : QObject(parent), m_name(name), m_value(value)
{
}

QString SimService::name() const
{
    return m_name;
}

void SimService::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
    }
}

QString SimService::value() const
{
    return m_value;
}

void SimService::setValue(const QString &value)
{
    if (value != m_value) {
        m_value = value;
    }
}
