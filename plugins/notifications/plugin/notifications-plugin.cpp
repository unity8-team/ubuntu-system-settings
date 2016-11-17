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

#include "notifications-plugin.h"

#include <QProcessEnvironment>
#include <QDebug>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

class NotificationsItem: public ItemBase
{
    Q_OBJECT
public:
    explicit NotificationsItem(const QVariantMap &staticData,
                               QObject *parent = 0);
    void setVisibility(bool visible);
};

NotificationsItem::NotificationsItem(const QVariantMap &staticData,
                                     QObject *parent):
    ItemBase(staticData, parent)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    setVisibility(!env.contains(QLatin1String("SNAP")));
}

void NotificationsItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *NotificationsPlugin::createItem(const QVariantMap &staticData,
                                          QObject *parent)
{
    return new NotificationsItem(staticData, parent);
}

#include "notifications-plugin.moc"
