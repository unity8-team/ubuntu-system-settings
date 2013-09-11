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

#include "click.h"

Click::Click(const QString &name,
             const QString &icon,
             const QString &install,
             QObject *parent) :
    QObject(parent),
    m_clickName(name),
    m_clickIcon(icon),
    m_clickInstall(install)

{

}

QString Click::clickName() const
{
    return m_clickName;
}

QString Click::clickIcon() const
{
    return m_clickIcon;

}
QString Click::clickInstall() const
{
    return m_clickInstall;

}
Click::~Click() {
}
