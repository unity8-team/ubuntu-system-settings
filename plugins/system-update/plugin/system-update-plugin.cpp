/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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
 *
 */

#include "system-update-plugin.h"

#include <QFile>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

typedef QMap<QString,QString> VersionDetail;
Q_DECLARE_METATYPE(VersionDetail)

class SystemUpdateItem: public ItemBase
{
    Q_OBJECT

public:
    explicit SystemUpdateItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);
};


SystemUpdateItem::SystemUpdateItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    auto sid = new QFile(QString("/usr/sbin/system-image-dbus"));
    setVisibility(sid->exists());
}

void SystemUpdateItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *SystemUpdatePlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new SystemUpdateItem(staticData, parent);
}

#include "system-update-plugin.moc"
