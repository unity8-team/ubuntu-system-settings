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

/* Look through `hooks' for a desktop or ini file in `directory'
 * and set the display name and icon from this.
 *
 * Will set with information from the first desktop or ini file found and parsed.
 */
void ClickModel::populateFromDesktopOrIniFile (Click *newClick,
                                          QVariantMap hooks,
                                          QDir directory)
{
    QVariantMap appHooks;
    GKeyFile *appinfo = g_key_file_new();
    gchar *desktopOrIniFileName = nullptr;

    QVariantMap::ConstIterator begin(hooks.constBegin());
    QVariantMap::ConstIterator end(hooks.constEnd());

    const gchar *keyGroup = G_KEY_FILE_DESKTOP_GROUP;
    const gchar *keyName = G_KEY_FILE_DESKTOP_KEY_NAME;

    // Look through the hooks for a 'desktop' key which points to a desktop
    // file referring to this app, or a 'scope' key pointing to an ini
    while (begin != end) {
        appHooks = (*begin++).toMap();
        if (!appHooks.isEmpty() &&
            (appHooks.contains("desktop") || appHooks.contains("scope")) &&
            directory.exists()) {

            if (appHooks.contains("scope"))
            {
                keyGroup = "ScopeConfig";
                keyName = "DisplayName";

                QDir scopeDirectory(
                            directory.absoluteFilePath(appHooks.value("scope", "").toString()));
                scopeDirectory.setNameFilters(QStringList()<<"*.ini");

                QStringList iniEntry(scopeDirectory.entryList());

                if (iniEntry.isEmpty())
                    goto out;

                QFile desktopOrIniFile(scopeDirectory.absoluteFilePath(iniEntry[0]));
                desktopOrIniFileName =
                        g_strdup(desktopOrIniFile.fileName().toLocal8Bit().constData());
                if (!desktopOrIniFile.exists())
                    goto out;
            }
            else
            {
                keyGroup = G_KEY_FILE_DESKTOP_GROUP;
                keyName = G_KEY_FILE_DESKTOP_KEY_NAME;

                QFile desktopOrIniFile(directory.absoluteFilePath(
                                      appHooks.value("desktop", "undefined").toString()));

                desktopOrIniFileName =
                   g_strdup(desktopOrIniFile.fileName().toLocal8Bit().constData());

                if (!desktopOrIniFile.exists())
                    goto out;
            }

            g_debug ("Desktop or ini file: %s", desktopOrIniFileName);



            gboolean loaded = g_key_file_load_from_file(appinfo,
                                                        desktopOrIniFileName,
                                                        G_KEY_FILE_NONE,
                                                        nullptr);

            if (!loaded) {
                g_warning ("Couldn't parse desktop or ini file %s", desktopOrIniFileName);
                goto out;
            }

            gchar * name = g_key_file_get_locale_string (appinfo,
                                                  keyGroup,
                                                  keyName,
                                                  nullptr,
                                                  nullptr);

            if (name) {
                g_debug ("Name is %s", name);
                newClick->displayName = name;
                g_free (name);
                name = nullptr;
            }

            // Overwrite the icon with the .desktop or ini file's one if we have it.
            // This is the one that the app scope displays so use that if we
            // can.
            gchar * icon = g_key_file_get_string (appinfo,
                                                  keyGroup,
                                                  G_KEY_FILE_DESKTOP_KEY_ICON,
                                                  nullptr);

            if (icon) {
                g_debug ("Icon is %s", icon);
                QFile iconFile(icon);
                if (iconFile.exists()) {
                    newClick->icon = icon;
                } else {
                    QString qIcon(QString::fromLocal8Bit(icon));
                    iconFile.setFileName(directory.absoluteFilePath(
                                QDir::cleanPath(qIcon)));
                    if (iconFile.exists())
                        newClick->icon = iconFile.fileName();
                    else if (QIcon::hasThemeIcon(qIcon)) // try the icon theme
                        newClick->icon = qIcon;
                }
            }
        }
out:
        g_free (desktopOrIniFileName);
        g_key_file_free (appinfo);
        return;
    }
}

ClickModel::Click ClickModel::buildClick(QVariantMap manifest)
{
    Click newClick;
    QDir directory;

    newClick.name = manifest.value("title",
                              gettext("Unknown title")).toString();

    // This key is the base directory where the click package is installed to.
    // We'll look for files relative to this.
    if (manifest.contains("_directory")) {
        directory = manifest.value("_directory", "/undefined").toString();
        // Set the icon from the click package. Might be a path or a reference to a themed icon.
        QString iconFile(manifest.value("icon", "undefined").toString());

        if (directory.exists()) {
            QFile icon(directory.absoluteFilePath(iconFile.simplified()));
            if (!icon.exists() && QIcon::hasThemeIcon(iconFile)) // try the icon theme
                newClick.icon = iconFile;
            else
                newClick.icon = icon.fileName();
        }

    }

    // "hooks" → title → "desktop" or "ini" / "icon"
    QVariant hooks(manifest.value("hooks"));

    if (hooks.isValid()) {
        QVariantMap allHooks(hooks.toMap());
        // The desktop or ini file contains an icon and the display name
        populateFromDesktopOrIniFile(&newClick, allHooks, directory);
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
