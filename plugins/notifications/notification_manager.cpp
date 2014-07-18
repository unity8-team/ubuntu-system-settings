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
#include <gio/gdesktopappinfo.h>

#include "notification_manager.h"
#include "notification_item.h"
#include <iostream>


#define CLICK_COMMAND "click"
#define BLACKLIST_CONFIG_SCHEMA_ID "com.ubuntu.touch.notifications"
#define BLACKLIST_KEY "popup-blacklist"


namespace NotificationsPlugin {

void app_data_from_desktop_id (const char* desktop_id, char **display_name, char **icon_fname) {
    GAppInfo* app_info = (GAppInfo*)g_desktop_app_info_new(desktop_id);
    if (app_info != NULL) {
        *display_name = g_strdup(g_app_info_get_display_name(app_info));
        GIcon* icon = g_app_info_get_icon (app_info);
        if (icon != NULL) {
            *icon_fname = g_icon_to_string (icon);
            // g_app_info_get_icon has "transfer none"
        }
        g_object_unref (app_info);
    }
}



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

    GVariant *blacklist = g_settings_get_value(m_pushSettings, BLACKLIST_KEY);
    GVariantIter *iter;
    g_variant_get (blacklist, "a(ss)", &iter);
    gchar *pkg;
    gchar *app;
    m_blacklist.clear();
    while (g_variant_iter_loop (iter, "(ss)", &pkg, &app)) {
        m_blacklist[QString(pkg)+"_"+app] = true;
    }
    g_variant_iter_free (iter);
    g_variant_unref (blacklist);

    // LOAD ALL THE APPS

    QString output(m_process.readAllStandardOutput());
    QJsonDocument document = QJsonDocument::fromJson(output.toUtf8());
    QJsonArray array = document.array();

    for (int i = 0; i < array.size(); i++) { // This iterates over packages
        QJsonObject object = array.at(i).toObject();

        QString pkgname = object.value("name").toString();
        QString version = object.value("version").toString();

        // This iterates over apps
        QVariantMap hooks = object.value("hooks").toObject().toVariantMap();
        QList<QString> keys = hooks.keys();

        // We need one app that has a push-helper key
        bool has_helper = true; // FIXME: this is just to check settings, should be false
        for (int j = 0; j < keys.size(); ++j) {
            QVariantMap hook = hooks.value(keys.at(j)).toMap();
            if (hook.contains("push-helper")) {
                has_helper = true;
            }
        }
        if (!has_helper) {
            continue;
        }

        // It has a helper, so add items for all entries that have
        // a "desktop" key.
        for (int j = 0; j < keys.size(); ++j) {
            QString key = pkgname+"_"+keys.at(j);
            QVariantMap hook = hooks.value(keys.at(j)).toMap();
            if (hook.contains("desktop")) {
                QString appid = key+"_"+version+".desktop";
                char *display_name;
                char *icon_fname;
                app_data_from_desktop_id(appid.toUtf8().constData(), &display_name, &icon_fname);
                NotificationItem *item = new NotificationItem();
                bool blacklisted = m_blacklist.contains(key);
                item->setItemData(QString(display_name), QString(icon_fname), blacklisted, key);
                g_free(display_name);
                g_free(icon_fname);
                m_model.append(QVariant::fromValue(item));
                connect(item, &NotificationItem::updateNotificationStatus,
                        this, &NotificationsManager::checkUpdates);
            }
        }
    }
    Q_EMIT modelChanged();
}

void NotificationsManager::checkUpdates(QString key, bool value)
{
    std::cout << "CU" << key.toStdString() << "->" << value << "\n\n";
    // Update the internal blacklist
    if (value) {
        if (!m_blacklist.contains(key)) {
            m_blacklist[key] = true;
        }
    } else {
        if (m_blacklist.contains(key)) {
            m_blacklist.remove(key);
        }
    }
    // Save the config settings
    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a(ss)"));
    QList<QString> keys = m_blacklist.keys();
    for (int j = 0; j < keys.size(); ++j) {
        // Keys are package_app
        QStringList splitted = keys.at(j).split("_");
        QString pkgname = splitted.at(0);
        QString appname = splitted.at(1);
        std::cout << "BL" << pkgname.toStdString() << "---" << appname.toStdString() << "\n\n";
        g_variant_builder_add(builder, "(ss)", pkgname.toUtf8().constData(), appname.toUtf8().constData());
    }
    GVariant *bl = g_variant_new("a(ss)", builder);
    g_variant_builder_unref (builder);
    g_settings_set_value(m_pushSettings, BLACKLIST_KEY, bl);
    g_variant_unref (bl);
}

}

