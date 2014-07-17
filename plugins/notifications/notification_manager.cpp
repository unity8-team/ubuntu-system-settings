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

#include <gio/gio.h>

#include "notification_manager.h"
#include "notification_item.h"
#include <QDebug>
#include <iostream>


#define CLICK_COMMAND "click"
#define BLACKLIST_CONFIG_SCHEMA_ID "com.ubuntu.touch.notifications"

namespace NotificationsPlugin {

// XXX: lots of code copied from the update plugin.
// XXX: and lots of it is also reimplemented differently
// XXX: in the about plugin!
// XXX: And all of it should be replaced with libclick calls
// XXX: instead of calling out to the click command

NotificationsManager::NotificationsManager(QObject *parent):
    QObject(parent),
    m_pushSettings(g_settings_new(BLACKLIST_CONFIG_SCHEMA_ID))
{
    QObject::connect(&m_process, SIGNAL(finished(int)),
                  this, SLOT(loadModel()));

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("CLICK_COMMAND", QString(CLICK_COMMAND));
    QStringList args("list");
    args << "--manifest";
    m_process.start(command, args);
}

NotificationsManager::~NotificationsManager()
{
    g_object_unref(m_pushSettings);
}

void NotificationsManager::loadModel()
{
    // Load the blacklist


    // LOAD ALL THE APPS
    // XXX: maybe this should only load the apps that have the right
    // apparmor bit to support push but that's not available in manifest

    QString output(m_process.readAllStandardOutput());
    QJsonDocument document = QJsonDocument::fromJson(output.toUtf8());
    QJsonArray array = document.array();

    int i;
    for (i = 0; i < array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        QString title = object.value("title").toString();
        QString icon = object.value("icon").toString(); //+"/"+object.value("icon").toString();
        if (icon.isEmpty()) {icon = "distributor-logo";};
        NotificationItem *item = new NotificationItem();
        item->setItemData(title, icon, false);
        m_model.append(QVariant::fromValue(item));
        connect(item, &NotificationItem::updateNotificationStatus,
                this, &NotificationsManager::checkUpdates);
    }
    Q_EMIT modelChanged();

}

void NotificationsManager::checkUpdates(QString id, bool value)
{
    std::cout << id.toStdString() << value;
}

}
