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

#ifndef DISPLAYS_H
#define DISPLAYS_H

#include <QAbstractListModel>
#include <QObject>

#include "display.h"

class DisplayListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DisplayListModel(QObject *parent = 0);
    ~DisplayListModel();

    enum DisplayRoles {
        ResolutionRole = Qt::UserRole + 1,
        OrientationRole,
        ScaleRole,
        StateRole
    };

    void addDisplay(Display* display);

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<Display*> m_displays;
};

#endif // DISPLAYS_H
