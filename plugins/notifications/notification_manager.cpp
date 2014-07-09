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
 * Diego Sarmentero <diego.sarmentero@canonical.com>
 *
*/

#include "notification_manager.h"
#include "notification_item.h"
#include <QDebug>

namespace NotificationsPlugin {

NotificationsManager::NotificationsManager(QObject *parent):
    QObject(parent)
{
    loadModel();
}

NotificationsManager::~NotificationsManager()
{
}

void NotificationsManager::loadModel()
{
    // LOAD ALL THE APPS THAT SUPPORT PUSH NOTIFICATIONS
    // EXAMPLE:
    NotificationItem *item = new NotificationItem();
    item->setItemData("App1", "icon", false);
    m_model.append(QVariant::fromValue(item));
    connect(item, &NotificationItem::updateNotificationStatus,
            this, &NotificationsManager::checkUpdates);

    NotificationItem *item2 = new NotificationItem();
    item2->setItemData("App2", "icon", true);
    m_model.append(QVariant::fromValue(item2));
    connect(item, &NotificationItem::updateNotificationStatus,
            this, &NotificationsManager::checkUpdates);

    NotificationItem *item3 = new NotificationItem();
    item3->setItemData("App3", "icon", false);
    m_model.append(QVariant::fromValue(item3));
    connect(item, &NotificationItem::updateNotificationStatus,
            this, &NotificationsManager::checkUpdates);

    Q_EMIT modelChanged();
}

void NotificationsManager::checkUpdates(QString id, bool value)
{
    qDebug() << id << value;
}

}
