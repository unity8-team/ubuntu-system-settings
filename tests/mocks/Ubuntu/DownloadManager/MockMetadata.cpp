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

#include "MockMetadata.h"
#include <QDebug>

MockMetadata::MockMetadata(QObject* parent)
    : QObject(parent)
    , m_map()
    , m_custom()
{
}

QString MockMetadata::title() const
{
    return m_map.value("title", "").toString();
}

void MockMetadata::setTitle(QString title)
{
    m_map.insert("title", title);
}

bool MockMetadata::showInIndicator() const
{
    return false;
}

void MockMetadata::setShowInIndicator(bool shown)
{

}

QVariantMap MockMetadata::map() const
{
    return QVariantMap();
}

QStringList MockMetadata::command() const
{
    return m_map.contains("command") ? m_map.value("command").toStringList():QStringList();
}

void MockMetadata::setCommand(const QStringList &command)
{
    m_map.insert("command", command);
}


QVariantMap MockMetadata::custom() const
{
    return m_custom;
}

void MockMetadata::setCustom(const QVariantMap &custom)
{
    m_custom = custom;
}
