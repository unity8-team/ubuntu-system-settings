/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
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

#include "hotspot-plugin.h"

#include <QDebug>
#include <SystemSettings/ItemBase>
#include <qofonomanager.h>

using namespace SystemSettings;

class HotspotItem: public ItemBase
{
    Q_OBJECT

public:
    explicit HotspotItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);

private Q_SLOTS:
    void shouldShow(QStringList);
};


HotspotItem::HotspotItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QOfonoManager *mm = new QOfonoManager(this);
    // Hide the plugin if there are no modems present
    setVisibility(mm->modems().length() > 0);
    QObject::connect(mm, SIGNAL(modemsChanged(QStringList)),
                     this, SLOT(shouldShow(QStringList)));

}

void HotspotItem::shouldShow(QStringList modems)
{
    setVisibility(modems.length() > 0);
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
