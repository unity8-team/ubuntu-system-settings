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
 * A model for storing and accessing displays.
 */

#ifndef DISPLAY_MODEL_H
#define DISPLAY_MODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QSharedPointer>

#include "display.h"

class DisplayListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DisplayListModel(QObject *parent = 0);
    ~DisplayListModel();

    enum Roles
    {
      // Qt::DisplayRole holds display name
      EnabledRole = Qt::UserRole,
      ConnectedRole,
      ModeRole,
      AvailableModesRole,
      OrientationRole
    };

    void addDisplay(QSharedPointer<Display> &display);

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QModelIndex index(int row, int column,
                      const QModelIndex & parent = QModelIndex()) const;

    QSharedPointer<Display> getDisplay(const int outputId) const;

    Q_INVOKABLE bool apply(const bool &enabled);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<QSharedPointer<Display> > m_displays;
};

#endif // DISPLAY_MODEL_H
