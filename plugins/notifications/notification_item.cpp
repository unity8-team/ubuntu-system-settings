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

#include "notification_item.h"

namespace NotificationsPlugin {

NotificationItem::NotificationItem(QObject *parent) :
    QObject(parent)
{
}

NotificationItem::~NotificationItem()
{
}

void NotificationItem::setItemData(QString title, QString icon, bool status)
{
    m_title = title;
    m_icon = icon;
    m_status = status;
    Q_EMIT titleChanged();
    Q_EMIT iconChanged();
    Q_EMIT statusChanged();
}

void NotificationItem::setStatus(bool status) {
    m_status = status;
    Q_EMIT statusChanged();
    Q_EMIT updateNotificationStatus(m_title, m_status);
}

}
