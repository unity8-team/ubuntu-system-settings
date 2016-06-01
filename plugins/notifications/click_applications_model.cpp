/*
 * Copyright (C) 2016 Canonical Ltd
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
*/

#include <gio/gdesktopappinfo.h>

#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include "click_applications_model.h"

#define GSETTINGS_APPS_SCHEMA_ID "com.ubuntu.notifications.settings.applications"
#define GSETTINGS_APPLICATIONS_KEY "applications"
#define GSETTINGS_NOTIFICATIONS_SCHEMA_ID "com.ubuntu.notifications.settings"
#define GSETTINGS_BASE_PATH "/com/ubuntu/NotificationSettings/"
#define GSETTINGS_ENABLE_NOTIFICATIONS_KEY "enable-notifications"
#define GSETTINGS_SOUNDS_NOTIFY_KEY "use-sounds-notifications"
#define GSETTINGS_VIBRATIONS_NOTIFY_KEY "use-vibrations-notifications"
#define GSETTINGS_BUBBLES_NOTIFY_KEY "use-bubbles-notifications"
#define GSETTINGS_LIST_NOTIFY_KEY "use-list-notifications"

ClickApplicationsModel::ClickApplicationsModel(QObject* parent)
    : QAbstractListModel(parent),
    m_applications(new QGSettings(GSETTINGS_APPS_SCHEMA_ID))
{
    populateModel();

    connect(m_applications.data(), SIGNAL(changed(const QString&)), SLOT(onApplicationsListChanged(const QString&)));

    m_checkMissingDesktopDataTimer = new QTimer(this);
    m_checkMissingDesktopDataTimer->setInterval(1000);
    connect(m_checkMissingDesktopDataTimer, SIGNAL(timeout()), SLOT(checkMissingDesktopData()));
}

ClickApplicationsModel::~ClickApplicationsModel()
{
}

QHash<int, QByteArray> ClickApplicationsModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles[DisplayName] = "displayName";
        roles[Icon] = "icon";
        roles[EnableNotifications] = "enableNotifications";
        roles[SoundsNotify] = "soundsNotify";
        roles[VibrationsNotify] = "vibrationsNotify";
        roles[BubblesNotify] = "bubblesNotify";
        roles[ListNotify] = "listNotify";
    }
    return roles;
}

int ClickApplicationsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_entries.count();
}

QVariant ClickApplicationsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const ClickApplicationEntry& entry = m_entries.at(index.row());
    switch (role) {
    case DisplayName:
        return entry.displayName;
    case Icon:
        return entry.icon;
    case EnableNotifications:
        return entry.enableNotifications;
    case SoundsNotify:
        return entry.soundsNotify;
    case VibrationsNotify:
        return entry.vibrationsNotify;
    case BubblesNotify:
        return entry.bubblesNotify;
    case ListNotify:
        return entry.listNotify;
    default:
        return QVariant();
    }
}

bool ClickApplicationsModel::setNotifyEnabled(int role, int idx, bool enabled)
{
    if (idx < 0 || idx >= rowCount()) {
        return false;
    }

    if (!saveNotifyEnabled(m_entries[idx], role, enabled)) {
        return false;
    }

    QVector<int> roles;
    roles << role;

    if (role != EnableNotifications) {
        if (!m_entries[idx].soundsNotify &&
            !m_entries[idx].vibrationsNotify &&
            !m_entries[idx].bubblesNotify &&
            !m_entries[idx].listNotify) {

            if (saveNotifyEnabled(m_entries[idx], EnableNotifications, false)) {
                roles << EnableNotifications;
            }
        }
    }

    Q_EMIT dataChanged(this->index(idx, 0), this->index(idx, 0), roles);
    return true;
}

bool ClickApplicationsModel::saveNotifyEnabled(ClickApplicationEntry& entry, int role, bool enabled)
{
    QString path = GSETTINGS_BASE_PATH;
 
    if (entry.appName.isEmpty()) {
        path = path + "dpkg/" + entry.pkgName + "/";
    } else {
        path = path + entry.pkgName + "/" + entry.appName + "/";
    }

    QScopedPointer<QGSettings> settings(new QGSettings(GSETTINGS_NOTIFICATIONS_SCHEMA_ID, path.toUtf8().constData()));

    switch (role) {
    case EnableNotifications:
        if (entry.enableNotifications == enabled) {
            return false;
        }

        entry.enableNotifications = enabled;
        settings->set(GSETTINGS_ENABLE_NOTIFICATIONS_KEY, enabled);
        return true;

    case SoundsNotify:
        if (entry.soundsNotify == enabled) {
            return false;
        }

        entry.soundsNotify = enabled;
        settings->set(GSETTINGS_SOUNDS_NOTIFY_KEY, enabled);
        return true;

    case VibrationsNotify:
        if (entry.vibrationsNotify == enabled) {
            return false;
        }

        entry.vibrationsNotify = enabled;
        settings->set(GSETTINGS_VIBRATIONS_NOTIFY_KEY, enabled);
        return true;

    case BubblesNotify:
        if (entry.bubblesNotify == enabled) {
            return false;
        }

        entry.bubblesNotify = enabled;
        settings->set(GSETTINGS_BUBBLES_NOTIFY_KEY, enabled);
        return true;

    case ListNotify:
        if (entry.listNotify == enabled) {
            return false;
        }

        entry.listNotify = enabled;
        settings->set(GSETTINGS_LIST_NOTIFY_KEY, enabled);
        return true;

    default:
        return false;
    }
}

bool ClickApplicationsModel::getApplicationDataFromDesktopFile(ClickApplicationEntry& entry)
{
    QString desktopFile = entry.pkgName + ".desktop";
    if (!entry.appName.isEmpty() && !entry.version.isEmpty()) {
        desktopFile = entry.pkgName + "_" + entry.appName + "_" + entry.version + ".desktop";
    }

    GAppInfo* appInfo = (GAppInfo*)g_desktop_app_info_new(desktopFile.toUtf8().constData());
    if (appInfo == nullptr) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to get desktop file:" << desktopFile;
        return false;
    }

    entry.displayName = g_strdup(g_app_info_get_display_name(appInfo));
    GIcon* icon = g_app_info_get_icon(appInfo);
    if (icon != nullptr) {
        QString iconPath = g_icon_to_string(icon);
        entry.icon = iconPath;
    }

    g_object_unref(appInfo);
    return true;
}

void ClickApplicationsModel::getNotificationsSettings(ClickApplicationEntry& entry)
{
    QString path = GSETTINGS_BASE_PATH;
 
    if (entry.appName.isEmpty()) {
        path = path + "dpkg/" + entry.pkgName + "/";
    } else {
        path = path + entry.pkgName + "/" + entry.appName + "/";
    }

    QScopedPointer<QGSettings> settings(new QGSettings(GSETTINGS_NOTIFICATIONS_SCHEMA_ID, path.toUtf8().constData()));

    entry.enableNotifications = settings->get(GSETTINGS_ENABLE_NOTIFICATIONS_KEY).toBool();
    entry.soundsNotify = settings->get(GSETTINGS_SOUNDS_NOTIFY_KEY).toBool();
    entry.vibrationsNotify = settings->get(GSETTINGS_VIBRATIONS_NOTIFY_KEY).toBool();
    entry.bubblesNotify = settings->get(GSETTINGS_BUBBLES_NOTIFY_KEY).toBool();
    entry.listNotify = settings->get(GSETTINGS_LIST_NOTIFY_KEY).toBool();
}

bool ClickApplicationsModel::parseApplicationKeyFromSettings(ClickApplicationEntry& entry, const QString& appEntry)
{
    QStringList entryData = appEntry.split('/');
    if (entryData.size() != 3) {
        return false;
    }

    if (entryData[0] == "dpkg" && entryData[2] == "0") {
        // Legacy dpkg application
        entry.pkgName = entryData[1];
    } else {
        entry.pkgName = entryData[0];
        entry.appName = entryData[1];
        entry.version = entryData[2];
    }

    return true;
}

int ClickApplicationsModel::getIndexByApplicationData(ClickApplicationEntry& entry)
{
    for (int i = 0; i < rowCount(); ++i) {
         if (m_entries.at(i).pkgName != entry.pkgName) {
             continue;
         }

         if (m_entries.at(i).appName != entry.appName) {
             continue;
         }

         return i;
    }

    return -1;
}

void ClickApplicationsModel::addMissingDesktopDataEntry(ClickApplicationEntry& entry)
{
    m_missingDesktopDataEntries << entry;
    m_checkMissingDesktopDataTimer->start();
}

void ClickApplicationsModel::addEntry(ClickApplicationEntry& entry)
{
    getNotificationsSettings(entry);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_entries << entry;
    endInsertRows();
    Q_EMIT rowCountChanged();
}

void ClickApplicationsModel::removeEntryByIndex(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_entries.removeAt(index);
    endRemoveRows();
    Q_EMIT rowCountChanged();
}

void ClickApplicationsModel::populateModel()
{
    Q_FOREACH (QString appEntry, m_applications->get(GSETTINGS_APPLICATIONS_KEY).toStringList()) {
        ClickApplicationEntry entry;
        if (!parseApplicationKeyFromSettings(entry, appEntry)) {
            continue;
        }

        if (!getApplicationDataFromDesktopFile(entry)) {
            addMissingDesktopDataEntry(entry);
            continue;
        }

        addEntry(entry);
    }
}

void ClickApplicationsModel::onApplicationsListChanged(const QString& key) {
    if (key != GSETTINGS_APPLICATIONS_KEY) {
        return;
    }

    //Check for removed entries
    for (int i = rowCount() - 1; i >= 0; --i) {
        bool removed = true;        

        Q_FOREACH (QString appEntry, m_applications->get(GSETTINGS_APPLICATIONS_KEY).toStringList()) {
            ClickApplicationEntry entry;
            if (!parseApplicationKeyFromSettings(entry, appEntry)) {
                continue;
            }

            if (m_entries.at(i).pkgName == entry.pkgName && m_entries.at(i).appName == entry.appName) {
                removed = false;
                continue;
            }
        }

        if (!removed) {
            continue;
        }

        removeEntryByIndex(i);
    }

    //Check for added entries
    Q_FOREACH (QString appEntry, m_applications->get(GSETTINGS_APPLICATIONS_KEY).toStringList()) {
        ClickApplicationEntry entry;
        if (!parseApplicationKeyFromSettings(entry, appEntry)) {
            continue;
        }

        if (getIndexByApplicationData(entry) >= 0) {
            continue;
        }

        if (!getApplicationDataFromDesktopFile(entry)) {
            addMissingDesktopDataEntry(entry);
            continue;
        }

        addEntry(entry);
    }
}

void ClickApplicationsModel::checkMissingDesktopData()
{
    QList<ClickApplicationEntry> stillMissing;

    while (!m_missingDesktopDataEntries.isEmpty()) {
        ClickApplicationEntry entry = m_missingDesktopDataEntries.takeFirst();

        if (!getApplicationDataFromDesktopFile(entry)) {
            stillMissing << entry;
        } else {
            addEntry(entry);
        }
    }

    if (stillMissing.isEmpty()) {
        m_checkMissingDesktopDataTimer->stop();
    } else {
        m_missingDesktopDataEntries.append(stillMissing);
    }
}
