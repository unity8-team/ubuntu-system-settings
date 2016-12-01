/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MockPluginManager.h"

MockPluginManager::MockPluginManager(QObject *parent) : QObject(parent)
{
}

QObject* MockPluginManager::getByName(const QString &name) const
{
    Q_UNUSED(name);
    return nullptr;
}

QAbstractItemModel* MockPluginManager::itemModel(const QString &category)
{
    Q_UNUSED(category);
    return nullptr;
}

void MockPluginManager::resetPlugins()
{
}

QString MockPluginManager::getFilter()
{
    return m_filter;
}

void MockPluginManager::setFilter(const QString &filter)
{
    m_filter = filter;
    Q_EMIT filterChanged();
}
