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

#include <click.h>
#include <gio/gdesktopappinfo.h>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>
#include <QtGui/QIcon>

#include "click_applications_model.h"
#include "click_application_entry.h"

#define LEGACY_PUSH_HELPER_DIR "/usr/lib/ubuntu-push-client/legacy-helpers/"

ClickApplicationsModel::ClickApplicationsModel(QObject* parent)
    : QAbstractListModel(parent)
{
    populateFromLegacyHelpersDir();
    populateFromClickDatabase();
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

    ClickApplicationEntry *entry = m_entries.at(index.row());
    switch (role) {
    case DisplayName:
        return entry->displayName();
    case Icon:
        return entry->icon();
    case SoundsNotify:
        return entry->soundsNotify();
    case VibrationsNotify:
        return entry->vibrationsNotify();
    case BubblesNotify:
        return entry->bubblesNotify();
    case ListNotify:
        return entry->listNotify();
    default:
        return QVariant();
    }
}

ClickApplicationEntry* ClickApplicationsModel::get(int index) const
{
    if (index < 0 || index >= rowCount()) {
        return nullptr;
    }

    return m_entries.at(index);
}

void ClickApplicationsModel::onEntrySoundsNotifyChanged()
{
    ClickApplicationEntry *entry = qobject_cast<ClickApplicationEntry*>(sender());
    notifyDataChanged(entry, SoundsNotify);
}

void ClickApplicationsModel::onEntryVibrationsNotifyChanged()
{
    ClickApplicationEntry *entry = qobject_cast<ClickApplicationEntry*>(sender());
    notifyDataChanged(entry, VibrationsNotify);
}

void ClickApplicationsModel::onEntryBubblesNotifyChanged()
{
    ClickApplicationEntry *entry = qobject_cast<ClickApplicationEntry*>(sender());
    notifyDataChanged(entry, BubblesNotify);
}

void ClickApplicationsModel::onEntryListNotifyChanged()
{
    ClickApplicationEntry *entry = qobject_cast<ClickApplicationEntry*>(sender());
    notifyDataChanged(entry, ListNotify);
}

void ClickApplicationsModel::notifyDataChanged(ClickApplicationEntry *entry, int role)
{
    int idx = m_entries.indexOf(entry);
    if (idx < 0) {
        return;
    }

    QVector<int> roles;
    roles << role;

    Q_EMIT dataChanged(this->index(idx, 0), this->index(idx, 0), roles);
}

ClickApplicationEntry* ClickApplicationsModel::getNewClickApplicationEntry()
{
    ClickApplicationEntry* newClick = new ClickApplicationEntry();

    connect(newClick, SIGNAL(soundsNotifyChanged()), SLOT(onEntrySoundsNotifyChanged()));
    connect(newClick, SIGNAL(vibrationsNotifyChanged()), SLOT(onEntryVibrationsNotifyChanged()));   
    connect(newClick, SIGNAL(bubblesNotifyChanged()), SLOT(onEntryBubblesNotifyChanged()));
    connect(newClick, SIGNAL(listNotifyChanged()), SLOT(onEntryListNotifyChanged()));

    return newClick;
}

void ClickApplicationsModel::addClickApplicationEntry(ClickApplicationEntry *entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_entries << entry;
    endInsertRows();
}

void ClickApplicationsModel::getApplicationDataFromDesktopFile(ClickApplicationEntry *entry)
{
    QString desktopFile = entry->pkgName() + ".desktop";
    if (!entry->appName().isEmpty() && !entry->version().isEmpty()) {
        desktopFile = entry->pkgName() + "_" + entry->appName() + "_" + entry->version() + ".desktop";
    }

    GAppInfo* appInfo = (GAppInfo*)g_desktop_app_info_new(desktopFile.toUtf8().constData());
    if (appInfo == nullptr) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to get desktop file:" << desktopFile;
        return;
    }

    entry->setDisplayName(g_strdup(g_app_info_get_display_name(appInfo)));
    GIcon* icon = g_app_info_get_icon(appInfo);
    if (icon != nullptr) {
        QString iconPath = g_icon_to_string(icon);
        entry->setIcon(iconPath);
    }

    g_object_unref(appInfo);
}

void ClickApplicationsModel::populateFromLegacyHelpersDir()
{
     QDirIterator it(LEGACY_PUSH_HELPER_DIR, QDir::Files, QDirIterator::NoIteratorFlags);
     while (it.hasNext()) {
         QFileInfo fileInfo(it.next());

         ClickApplicationEntry *entry = getNewClickApplicationEntry();
         entry->setPkgName(fileInfo.fileName());

         getApplicationDataFromDesktopFile(entry);
         addClickApplicationEntry(entry);
     }
}

bool ClickApplicationsModel::clickManifestHasPushHelperHook(const QVariantMap& manifest)
{
    QVariant hooksVar(manifest.value("hooks"));
    if (!hooksVar.isValid()) {
        return false;
    }

    QVariantMap hooksMap(hooksVar.toMap());
    QMapIterator<QString, QVariant> it(hooksMap);
    while (it.hasNext()) {
        it.next();
        QVariant hookVar(it.value());

        if (hookVar.isValid()) {
            QVariantMap hookMap(hookVar.toMap());
            if (hookMap.keys().contains("push-helper")) {
                return true;
            }
        }
    }

    return false;
}

QString ClickApplicationsModel::getApplicationNameFromDesktopHook(const QVariantMap& manifest)
{
    QVariant hooksVar(manifest.value("hooks"));
    if (!hooksVar.isValid()) {
        return QString();
    }

    QVariantMap hooksMap(hooksVar.toMap());
    QMapIterator<QString, QVariant> it(hooksMap);
    while (it.hasNext()) {
        it.next();
        QVariant hookVar(it.value());

        if (hookVar.isValid()) {
            QVariantMap hookMap(hookVar.toMap());
            if (hookMap.keys().contains("desktop")) {
                return it.key();
            }
        }
    }

    return QString();
}

void ClickApplicationsModel::populateFromClickDatabase()
{
    ClickDB *clickdb = click_db_new();

    GError *err = nullptr;
    click_db_read(clickdb, nullptr, &err);
    if (err != nullptr) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to read click packages db:" << err->message;
        g_error_free(err);
        g_object_unref(clickdb);
        return;
    }

    gchar *clickmanifest = click_db_get_manifests_as_string(clickdb, FALSE, &err);
    if (err != nullptr) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to get click packages manifest:" << err->message;
        g_error_free(err);
        g_object_unref(clickdb);
        return;
    }

    g_object_unref(clickdb);

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(clickmanifest, &jsonError);

    g_free(clickmanifest);

    if (jsonError.error != QJsonParseError::NoError) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to parser data from click packages manifest:" << jsonError.errorString();
        return;
    }

    QJsonArray data(jsonDoc.array());
    QJsonArray::ConstIterator it;
    for (it = data.constBegin(); it != data.constEnd(); ++it) {
        QVariantMap manifest = (*it).toObject().toVariantMap();

        if (!clickManifestHasPushHelperHook(manifest)) {
            continue;
        }

        ClickApplicationEntry *entry = getNewClickApplicationEntry();
        entry->setPkgName(manifest.value("name").toString());
        entry->setVersion(manifest.value("version").toString());
        entry->setAppName(getApplicationNameFromDesktopHook(manifest));

        entry->setDisplayName(manifest.value("title").toString());
        entry->setIcon(manifest.value("icon").toString());

        getApplicationDataFromDesktopFile(entry);
        addClickApplicationEntry(entry);
    }
}
