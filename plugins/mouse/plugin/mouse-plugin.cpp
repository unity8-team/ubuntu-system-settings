/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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

#include "mouse-plugin.h"

#include <QDebug>
#include <QDBusInterface>
#include <QStringList>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

class MouseItem: public ItemBase
{
    Q_OBJECT

public:
    explicit MouseItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);

};


MouseItem::MouseItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QDBusConnection m_systemBusConnection (QDBusConnection::systemBus());
    QDBusInterface m_unityInterface ("com.canonical.unity",
                                     "/com/canonical/unity",
                                     "com.canonical.unity",
                                     m_systemBusConnection);

    /* FIXME: Determine visibility based on connection of mouse or touchpad */
    setVisibility(true);
}

void MouseItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *MousePlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new MouseItem(staticData, parent);
}

#include "mouse-plugin.moc"
