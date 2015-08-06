/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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

#include "hotspot-plugin.h"

#include <QDebug>
#include <QDBusInterface>
#include <QStringList>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

class HotspotItem: public ItemBase
{
    Q_OBJECT

public:
    explicit HotspotItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);

};


HotspotItem::HotspotItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QDBusInterface m_connectivityPrivateIface ("com.ubuntu.connectivity1",
                                  "/com/ubuntu/connectivity1/Private",
                                  "com.ubuntu.connectivity1.Private",
                                  QDBusConnection::sessionBus());

    // Hide the plugin if the private connectivity API is not provided
    setVisibility(m_connectivityPrivateIface.isValid());
    qWarning() << "abasbdasd";
}

void HotspotItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *HotspotPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new HotspotItem(staticData, parent);
}

#include "hotspot-plugin.moc"
