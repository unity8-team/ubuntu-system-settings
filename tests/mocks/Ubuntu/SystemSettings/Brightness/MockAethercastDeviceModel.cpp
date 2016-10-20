/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "MockAethercastDeviceModel.h"

int MockAethercastDeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_devices.size();
}

QVariant MockAethercastDeviceModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if ((0<=index.row()) && (index.row()<m_devices.size())) {
        auto device = m_devices[index.row()];
        QString displayName;

        switch (role) {
        case Qt::DisplayRole:
            displayName = device->name();

            if (displayName.isEmpty())
                displayName = device->address();

            ret = displayName;
            break;

        case StateRole:
            ret = device->state();
            break;

        case AddressRole:
            ret = device->address();
            break;

        }
    }
    return ret;
}

QHash<int,QByteArray> MockAethercastDeviceModel::roleNames() const
{
    static QHash<int,QByteArray> names;
    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[StateRole] = "stateName";
        names[AddressRole] = "addressName";
    }
    return names;
}

QSharedPointer<MockAethercastDevice> MockAethercastDeviceModel::getDeviceFromAddress(const QString &address)
{
    Q_UNUSED(address)
    return QSharedPointer<MockAethercastDevice>(nullptr);
}

QSharedPointer<MockAethercastDevice> MockAethercastDeviceModel::getDeviceFromPath(const QString &path)
{
    Q_UNUSED(path)
    return QSharedPointer<MockAethercastDevice>(nullptr);
}

void MockAethercastDeviceModel::addDevice(const QString &address,
                                          const QString &name,
                                          const int &state)
{
    QSharedPointer<MockAethercastDevice> dev =
        QSharedPointer<MockAethercastDevice>(new MockAethercastDevice);
    dev->m_address = address;
    dev->m_name = name;
    dev->m_state = (MockAethercastDevice::State) state;

    uint row = m_devices.size();
    beginInsertRows(QModelIndex(), row, row);
    m_devices.append(dev);
    endInsertRows();

    emit countChanged(rowCount());
}
