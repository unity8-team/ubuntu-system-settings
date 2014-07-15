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

#ifndef PREVIOUSNETWORKMODEL_H
#define PREVIOUSNETWORKMODEL_H

#include<QAbstractListModel>

class PreviousNetworkModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PreviousNetworkRoles {
        NameRole = Qt::UserRole + 1,
        ObjectPathRole,
        PasswordRole,
        LastUsedRole,
    };

    PreviousNetworkModel(QObject *parent = 0);
    virtual ~PreviousNetworkModel();
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex & index, int role) const;

private:
    struct Private;
    Private *p;
};

#endif
