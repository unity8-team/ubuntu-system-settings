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
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtGui/QIcon>

#include <libintl.h>
QString _(const char *text){
    return QString::fromUtf8(dgettext(0, text));
}

ClickApplicationsModel::ClickApplicationsModel(QObject* parent)
    : QAbstractListModel(parent)
{
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

void ClickApplicationsModel::populateFromManifest(ClickApplicationEntry &entry, const QVariantMap &var)
{
    entry.displayName = var.value("title", _("Unknown title")).toString();

    if (var.contains("_directory")) {
        QString dirPath(var.value("_directory", "").toString());
        QString iconFileName(var.value("icon", "").toString());
 
        if (!dirPath.isEmpty() && !iconFileName.isEmpty()) {
            QDir dir(dirPath);
            QFile icon(dir.absoluteFilePath(iconFileName.simplified()));

            if (!icon.exists() && QIcon::hasThemeIcon(iconFileName)) {
                entry.icon = QString("image://theme/%1").arg(iconFileName);
            } else {
                entry.icon = icon.fileName();
            }
        }
    }
}

void ClickApplicationsModel::populateFromDesktopFile(ClickApplicationEntry &entry, const QVariantMap &var, const QVariantMap &hooks)
{

}

void ClickApplicationsModel::populateFromClickDatabase()
{
    ClickDB *clickdb = click_db_new();

    GError *err = nullptr;
    click_db_read(clickdb, nullptr, &err);
    if (err != nullptr) {
        qWarning() << Q_FUNC_INFO << "Unable to read click packages db:" << err->message;
        g_error_free(err);
        g_object_unref(clickdb);
        return;
    }

    gchar *clickmanifest = click_db_get_manifests_as_string(clickdb, FALSE, &err);
    if (err != nullptr) {
        qWarning() << Q_FUNC_INFO << "Unable to get click packages manifest:" << err->message;
        g_error_free(err);
        g_object_unref(clickdb);
        return;
    }

    g_object_unref(clickdb);

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(clickmanifest, &jsonError);

    g_free(clickmanifest);

    if (jsonError.error != QJsonParseError::NoError) {
        qWarning() << Q_FUNC_INFO << "Unable to parser data from click packages manifest:" << jsonError.errorString();
        return;
    }

    int count = 0;
    QJsonArray data(jsonDoc.array());
    QJsonArray::ConstIterator i;
    for (i = data.constBegin(); i != data.constEnd(); ++i) {
        QVariantMap var = (*i).toObject().toVariantMap();

        ClickApplicationEntry entry;
        populateFromManifest(entry, var);

        /*
        QVariant hooksVar(var.value("hooks"));
        if (hooksVar.isValid()) {
            QVariantMap hooks(hooksVar.toMap());
            populateFromDesktopFile(entry, var, hooks);
        }
        */

        beginInsertRows(QModelIndex(), count, count);
        m_entries.append(entry);
        endInsertRows();
        ++count;
    }
}
