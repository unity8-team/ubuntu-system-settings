/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Authors: Iain Lane <iain.lane@canonical.com>
 *
*/

#include "click.h"

#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
#include <glib.h>
#include <libintl.h>

#include <QDebug>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

ClickModel::ClickModel(QObject *parent):
    QAbstractTableModel(parent),
    m_totalClickSize(0)
{
    m_clickPackages = buildClickList();
}

/* Look through `hooks' for a desktop file in `directory'
 * and set the display name and icon from this.
 *
 * Will set with information from the first desktop file found and parsed.
 */
void ClickModel::populateFromDesktopFile (Click *newClick,
                                          QVariantMap hooks,
                                          QDir directory)
{
    QVariantMap appHooks;

    QVariantMap::ConstIterator begin(hooks.constBegin());
    QVariantMap::ConstIterator end(hooks.constEnd());

    while (begin != end) {
        appHooks = (*begin++).toMap();
        if (!appHooks.isEmpty() &&
            appHooks.contains("desktop") &&
            directory.exists()) {
            QFile desktopFile(
                        directory.absoluteFilePath(
                            appHooks.value("desktop",
                                           "undefined").toString()));

            gchar * desktopFileName =
                g_strdup(desktopFile.fileName().toLocal8Bit().constData());

            g_debug ("Desktop file: %s", desktopFileName);
            if (desktopFile.exists()) {
                GDesktopAppInfo *appinfo =
                        g_desktop_app_info_new_from_filename (
                            desktopFileName);

                if (!appinfo) {
                    g_warning ("Couldn't parse desktop file %s",
                               desktopFileName);
                    g_free (desktopFileName);
                    return;
                }

                const char * name = g_app_info_get_name (
                            (GAppInfo *) appinfo);

                if (name) {
                    g_debug ("Name is %s", name);
                    newClick->displayName = name;
                }

                // Overwrite the icon with the .desktop file's one
                // if we have it. This one is more reliable.
                const char * icon = g_desktop_app_info_get_string (
                            appinfo, "Icon");
                if (icon) {
                    g_debug ("Icon is %s", icon);
                    QFile iconFile(icon);
                    if (iconFile.exists()) {
                        newClick->icon = icon;
                    } else {
                        iconFile.setFileName(
                                    directory.absoluteFilePath(
                                        QDir::cleanPath(
                                            QString::fromLocal8Bit(icon))));
                        if (iconFile.exists())
                            newClick->icon = iconFile.fileName();
                    }
                }
            }
            g_free (desktopFileName);
            return;
        }
    }
}

ClickModel::Click ClickModel::buildClick(QVariantMap manifest)
{
    Click newClick;
    QDir directory;

    newClick.name = manifest.value("title",
                              gettext("Unknown title")).toString();

    if (manifest.contains("_directory")) {
        directory = manifest.value("_directory", "/undefined").toString();
        // Set the icon from the click package
        QString iconFile(manifest.value("icon", "undefined").toString());
        if (directory.exists()) {
            QString icon(directory.filePath(iconFile.simplified()));
            newClick.icon = icon;
        }
    }

    // "hooks" → title → "desktop" / "icon"
    QVariant hooks(manifest.value("hooks"));

    if (hooks.isValid()) {
        QVariantMap allHooks(hooks.toMap());
        populateFromDesktopFile(&newClick, allHooks, directory);
   }

    newClick.installSize = manifest.value("installed-size",
                                     "0").toString().toUInt()*1024;

    m_totalClickSize += newClick.installSize;

    return newClick;
}

QList<ClickModel::Click> ClickModel::buildClickList()
{
    QFile clickBinary("/usr/bin/click");
    if (!clickBinary.exists()) {
        return QList<ClickModel::Click>();
    }

    QProcess clickProcess;
    clickProcess.start("/usr/bin/click",
                       QStringList() << "list" << "--all" << "--manifest");

    if (!clickProcess.waitForFinished(5000)) {
        qWarning() << "Timeout retrieving the list of click packages";
        return QList<ClickModel::Click>();
    }

    if (clickProcess.exitStatus() == QProcess::CrashExit) {
        qWarning() << "The click utility exited abnormally" <<
                      clickProcess.readAllStandardError();
        return QList<ClickModel::Click>();
    }

    QJsonParseError error;

    QJsonDocument jsond =
            QJsonDocument::fromJson(clickProcess.readAllStandardOutput(),
                                    &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << error.errorString();
        return QList<ClickModel::Click>();
    }

    QJsonArray data(jsond.array());

    QJsonArray::ConstIterator begin(data.constBegin());
    QJsonArray::ConstIterator end(data.constEnd());

    QList<ClickModel::Click> clickPackages;

    while (begin != end) {
        QVariantMap val = (*begin++).toObject().toVariantMap();

        clickPackages.append(buildClick(val));
    }

    return clickPackages;
}

int ClickModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_clickPackages.count();
}

int ClickModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 3; //Display, size, icon
}

QHash<int, QByteArray> ClickModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;

    roleNames[Qt::DisplayRole] = "displayName";
    roleNames[InstalledSizeRole] = "installedSize";
    roleNames[IconRole] = "iconPath";

    return roleNames;
}

QVariant ClickModel::data(const QModelIndex &index, int role) const
{
    if (index.row() > m_clickPackages.count() ||
            index.row() < 0)
        return QVariant();

    Click click = m_clickPackages[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        if (click.displayName.isEmpty() || click.displayName.isNull())
            return click.name;
        else
            return click.displayName;
    case InstalledSizeRole:
        return click.installSize;
    case IconRole:
        return click.icon;
    default:
        qWarning() << "Unknown role requested";
        return QVariant();
    }
}

quint64 ClickModel::getClickSize() const
{
    return m_totalClickSize;
}

ClickModel::~ClickModel()
{
}

ClickFilterProxy::ClickFilterProxy(ClickModel *parent)
    : QSortFilterProxyModel(parent)
{
    this->setSourceModel(parent);
    this->setDynamicSortFilter(false);
    this->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->sort(0);
}
