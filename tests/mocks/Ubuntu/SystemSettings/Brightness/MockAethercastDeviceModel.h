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

#ifndef MOCK_AETHERCAST_DEVICE_MODEL_H
#define MOCK_AETHERCAST_DEVICE_MODEL_H

#include "MockAethercastDevice.h"

#include <QAbstractListModel>
#include <QList>
#include <QObject>
#include <QSharedPointer>

class MockAethercastDeviceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    MockAethercastDeviceModel(QObject *parent = 0) {
        Q_UNUSED(parent)
    };
    ~MockAethercastDeviceModel() {};

    enum Roles
    {
      // Qt::DisplayRole holds device name
      TypeRole = Qt::UserRole,
      AddressRole,
      StateRole,
      LastRole = StateRole
    };
    // implemented virtual methods from QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

    QSharedPointer<MockAethercastDevice> getDeviceFromAddress(const QString &address);
    QSharedPointer<MockAethercastDevice> getDeviceFromPath(const QString &path);
    QList<QSharedPointer<MockAethercastDevice> > m_devices;

Q_SIGNALS:
    void countChanged(int count);
};

#endif // MOCK_AETHERCAST_DEVICE_MODEL_H
