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

#include "click_applications_model.h"

#include <click.h>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>
#include <QtGui/QIcon>

#define LEGACY_PUSH_HELPER_DIR "/usr/lib/ubuntu-push-client/legacy-helpers/"

#include <libintl.h>
QString _(const char *text){
    return QString::fromUtf8(dgettext(0, text));
}

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
    default:
        return QVariant();
    }
}

void ClickApplicationsModel::addClickApplicationEntry(const ClickApplicationEntry& entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_entries.append(entry);
    endInsertRows();
}

void ClickApplicationsModel::populateFromLegacyHelpersDir()
{
     QDirIterator it(LEGACY_PUSH_HELPER_DIR, QDir::Files, QDirIterator::NoIteratorFlags);
     while (it.hasNext()) {
         QFileInfo fileInfo(it.next());

         ClickApplicationEntry entry;
         entry.pkgName = fileInfo.fileName();
         entry.displayName = entry.pkgName; //FIXME
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
    QMapIterator<QString, QVariant> hooksIterator(hooksMap);
    while (hooksIterator.hasNext()) {
        hooksIterator.next();
        QVariant hookVar(hooksIterator.value());

        if (hookVar.isValid()) {
            QVariantMap hookMap(hookVar.toMap());
            if (hookMap.keys().contains("push-helper")) {
                return true;
            }
        }
    }

    return false;
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
    QJsonArray::ConstIterator i;
    for (i = data.constBegin(); i != data.constEnd(); ++i) {
        QVariantMap manifest = (*i).toObject().toVariantMap();

        if (!clickManifestHasPushHelperHook(manifest)) {
            continue;
        }

        ClickApplicationEntry entry;
        entry.pkgName = manifest.value("name").toString();
        entry.version = manifest.value("version").toString();

        entry.displayName = manifest.value("title").toString();
        entry.icon = manifest.value("icon").toString();


        entry.displayName = entry.pkgName; //FIXME
        addClickApplicationEntry(entry);
    }
}
