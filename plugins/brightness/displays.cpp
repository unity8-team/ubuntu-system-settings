/*
 * Copyright (C) 2015 Canonical Ltd
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
 */

#include <QDebug>
#include "displays.h"

DisplayListModel::DisplayListModel(QObject *parent)
    : QAbstractListModel(parent) {
}

void DisplayListModel::addDisplay(Display* display) {
    int at = rowCount();
    for (int i = 0; i < m_displays.size(); ++i) {
        if (display->path() < m_displays.at(i)->path()) {
            at = i;
            break;
        }
    }
    beginInsertRows(QModelIndex(), at, at);
    m_displays.insert(at, display);
    endInsertRows();
}

int DisplayListModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_displays.count();
}

QVariant DisplayListModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_displays.count())
        return QVariant();

    Display* display = m_displays[index.row()];
    if (role == Qt::DisplayRole) {
        return QVariant::fromValue(display);
    }
    //  else if (role == displayRole) {
    //     return QVariant::fromValue(display);
    // } else if (role == GroupRole) {
    //     return display->path();;
    // }
    // if (role == TypeRole)
    //     return display.type();
    // else if (role == SizeRole)
    //     return display.size();
    return QVariant();
}

QHash<int, QByteArray> DisplayListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ResolutionRole] = "resolution";
    roles[OrientationRole] = "orientation";
    roles[ScaleRole] = "scale";
    roles[StateRole] = "state";
    return roles;
}

DisplayListModel::~DisplayListModel() {
    qDeleteAll(m_displays);
}
