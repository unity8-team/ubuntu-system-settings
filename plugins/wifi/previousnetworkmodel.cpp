/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "previousnetworkmodel.h"
#include "wifidbushelper.h"
#include<QVariant>

struct PreviousNetworkModel::Private {
    QList<QStringList> data;
};

PreviousNetworkModel::PreviousNetworkModel(QObject *parent) : QAbstractListModel(parent) {
    p = new PreviousNetworkModel::Private();

    WifiDbusHelper h;
    auto networks = h.getPreviouslyConnectedWifiNetworks();

    p->data = networks;
}

PreviousNetworkModel::~PreviousNetworkModel() {
    delete p;
}

QHash<int, QByteArray> PreviousNetworkModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ObjectPathRole] = "objectPath";
    roles[PasswordRole] = "password";
    roles[LastUsedRole] = "lastUsed";
    return roles;
}

int PreviousNetworkModel::rowCount(const QModelIndex &/*parent*/) const {
    return p->data.size();
}

QVariant PreviousNetworkModel::data(const QModelIndex & index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }

    const auto &row = p->data[index.row()];

    switch(role) {

    case NameRole : return QVariant(row[0]);
    case ObjectPathRole : return QVariant(row[1]);
    case PasswordRole : return QVariant(row[2]);
    case LastUsedRole : return QVariant(row[3]);

    default : return QVariant();

    }

}
