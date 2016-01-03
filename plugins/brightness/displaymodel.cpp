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
#include "displaymodel.h"

DisplayListModel::DisplayListModel(QObject *parent)
    : QAbstractListModel(parent) {
}

void DisplayListModel::addDisplay(QSharedPointer<Display> &display) {
    int at = rowCount();
    for (int i = 0; i < m_displays.size(); ++i) {
        if (display->id() < m_displays.at(i)->id()) {
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
    QVariant ret;
    if (index.row() < 0 || index.row() >= m_displays.count())
        return ret;

    auto display = m_displays[index.row()];
    switch (role) {
    case Qt::DisplayRole:
        ret = QString("Display %1").arg(QString::number(display->id()));
        break;
    case EnabledRole:
        ret = display->enabled();
        break;
    case ConnectedRole:
        ret = display->connected();
        break;
    case ModeRole:
        ret = display->mode();
        break;
    case AvailableModesRole:
        ret = display->availableModes();
        break;
    case OrientationRole:
        ret = display->orientation();
        break;
    }
    return ret;
}

bool DisplayListModel::setData(const QModelIndex &index,
                               const QVariant &value,
                               int role)
{
    if (index.row() < 0 || index.row() >= m_displays.count())
        return false;
    auto display = m_displays[index.row()];
    switch (role) {
    case EnabledRole:
        switch (static_cast<QMetaType::Type>(value.type())) {
        case QMetaType::Bool:
        case QMetaType::QChar:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            display->setEnabled(value.toBool());
            return true;

        default:
            break;
        }

        break;
    }

    return false;
}

QHash<int, QByteArray> DisplayListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[EnabledRole] = "enabled";
    roles[ConnectedRole] = "connected";
    roles[ModeRole] = "mode";
    roles[AvailableModesRole] = "availableModes";
    roles[OrientationRole] = "orientation";
    // roles[ScaleRole] = "scale";
    return roles;
}

QSharedPointer<Display> DisplayListModel::getDisplay(const int outputId) const {
    QSharedPointer<Display> display;

    for (int i=0, n=m_displays.size(); i<n; i++)
        if (m_displays[i]->id() == outputId)
            return m_displays[i];

    return display;
}

QModelIndex DisplayListModel::index(int row, int column,
        const QModelIndex & parent) const {
    Q_UNUSED(parent);
    Q_UNUSED(column);
    return createIndex(row, column);
}

DisplayListModel::~DisplayListModel() {
}
