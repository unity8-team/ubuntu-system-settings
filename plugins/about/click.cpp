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

#include <libintl.h>

#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

ClickModel::ClickModel(QObject *parent):
    QAbstractTableModel(parent)
{
    m_clickPackages = buildClickList();
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
    clickProcess.waitForFinished(-1);

    QJsonDocument jsond =
            QJsonDocument::fromJson(clickProcess.readAllStandardOutput());

    QJsonArray data(jsond.array());

    QJsonArray::ConstIterator begin(data.constBegin());
    QJsonArray::ConstIterator end(data.constEnd());

    QList<ClickModel::Click> clickPackages;

    while (begin != end) {
        QVariantMap val = (*begin++).toObject().toVariantMap();
        Click newClick;

        newClick.name = val.value("title",
                                  gettext("Unknown title")).toString();

        if (val.contains("_directory")) {
            QDir directory(val.value("_directory", "/undefined").toString());
            QString iconFile(val.value("icon", "undefined").toString());
            if (directory.exists()) {
                QString icon(directory.filePath(iconFile.simplified()));
                newClick.icon = icon;
            }
        }

        newClick.installSize = val.value("installed-size",
                                         "0").toString().toUInt();

        clickPackages.append(newClick);
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
        return click.name;
    case InstalledSizeRole:
        return click.installSize;
    case IconRole:
        return click.icon;
    default:
        return QVariant();
    }
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
