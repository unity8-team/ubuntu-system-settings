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

const QString nm_settings_connection("org.freedesktop.NetworkManager.Settings.Connection");
const QString nm_settings_connection_removed_member("Removed");

struct PreviousNetworkModel::Private {
    QList<QStringList> data;
};

PreviousNetworkModel::PreviousNetworkModel(QObject *parent) : QAbstractListModel(parent) {
    p = new PreviousNetworkModel::Private();

    const QString service("");
    const QString path("");

    QDBusConnection::systemBus().connect(
        service,
        path,
        nm_settings_connection,
        nm_settings_connection_removed_member,
        this,
        SLOT(removeConnection()));

    WifiDbusHelper h;
    auto networks = h.getPreviouslyConnectedWifiNetworks();
    p->data = networks;

}

void PreviousNetworkModel::removeConnection()
{
    qWarning() << "Received Removed signal from a connection object.";
    WifiDbusHelper h;
    QList<QStringList> networks = h.getPreviouslyConnectedWifiNetworks();

    int row = -1;
    for (int i=0, n=networks.length(); row==-1 && i<n; i++) {
        qWarning() << "Comparing" << networks[i][1] << "and" << p->data.at(i)[1];
        if (networks[i][1] != p->data.at(i)[1]) {
            qWarning() << "The two list of networks differed at" << i;
            row = i;
        }
    }

    if (0<=row && row<p->data.size()) {
        qWarning() << "Began removing" << row;
        beginRemoveRows(QModelIndex(), row, row);
        p->data.removeAt(row);
        endRemoveRows();
        qWarning() << "Finished removing" << row;
    }
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
    if(!index.isValid() || index.row() >= p->data.size()) {
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
