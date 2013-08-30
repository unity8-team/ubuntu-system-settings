/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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

#include "subset-model.h"

SubsetModel::SubsetModel(const QStringList &universe,
                         QObject           *parent) :
    QAbstractItemModel(parent),
    _universe(universe)
{
}

const QStringList &
SubsetModel::universe() const
{
    return _universe;
}

const QList<int> &
SubsetModel::subset() const
{
    return _subset;
}

void
SubsetModel::setSubset(const QList<int> &list)
{
    if (list != _subset)
    {
        beginResetModel();
        _subset = list;
        endResetModel();

        Q_EMIT subsetChanged();
    }
}

QModelIndex
SubsetModel::index(int                row,
                   int                column,
                   const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
    {
        if (!parent.isValid())
        {
            if (0 <= row && row < 2 && column == 0)
                return createIndex(row, column);
        }
        else
        {
            switch (parent.row())
            {
            case 0:
                if (0 <= row && row < _subset.length() && column == 0)
                    return createIndex(row, column, const_cast<void *>(static_cast<const void *>(&_subset)));

                break;

            case 1:
                if (0 <= row && row < _universe.length() && column == 0)
                    return createIndex(row, column, const_cast<void *>(static_cast<const void *>(&_universe)));

                break;
            }
        }
    }

    return QModelIndex();
}

QModelIndex
SubsetModel::parent(const QModelIndex &index) const
{
    if (index.isValid())
    {
        if (index.internalPointer() == &_subset)
            return createIndex(0, 0);
        else if (index.internalPointer() == &_universe)
            return createIndex(1, 0);
    }

    return QModelIndex();
}

int
SubsetModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 2;

    if (parent.internalPointer() == NULL)
    {
        switch (parent.row())
        {
        case 0:
            return _subset.length();

        case 1:
            return _universe.length();
        }
    }

    return 0;
}

int
SubsetModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() || (parent.internalPointer() == NULL && 0 <= parent.row() && parent.row() < 2);
}

QHash<int, QByteArray>
SubsetModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;

    roleNames.insert(Qt::DisplayRole, "text");
    roleNames.insert(Qt::CheckStateRole, "checked");

    return roleNames;
}

Qt::ItemFlags
SubsetModel::flags(const QModelIndex &index) const
{
    return index.isValid() && index.internalPointer() != NULL ? Qt::ItemIsUserCheckable : Qt::NoItemFlags;
}

QVariant
SubsetModel::data(const QModelIndex &index,
                  int                role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        if (index.internalPointer() == &_subset)
            return _universe[_subset[index.row()]];
        else if (index.internalPointer() == &_universe)
            return _universe[index.row()];

        break;

    case Qt::CheckStateRole:
        if (index.internalPointer() == &_subset)
            return true;
        else if (index.internalPointer() == &_universe)
            return _subset.contains(index.row());

        break;
    }

    return QVariant();
}

bool
SubsetModel::setData(const QModelIndex &index,
                     const QVariant    &value,
                     int                role)
{
    if (index.isValid() && index.internalPointer() != NULL)
    {
        switch (role)
        {
        case Qt::CheckStateRole:
            if ((QMetaType::Type) value.type() == QMetaType::Bool)
            {
                if (index.internalPointer() == &_subset)
                {
                    if (0 <= index.row() && index.row() < _subset.length())
                    {
                        if (!value.toBool())
                        {
                            int changed = _subset[index.row()];

                            for (int i = 0; i < _subset.length(); i++)
                            {
                                while (i < _subset.length() && _subset[i] == changed)
                                {
                                    beginRemoveRows(this->index(0, 0), i, i);
                                    _subset.removeAt(i);
                                    endRemoveRows();
                                }
                            }

                            QModelIndex unchecked(this->index(changed, 0, this->index(1, 0)));
                            Q_EMIT dataChanged(unchecked, unchecked, QVector<int>(1, Qt::CheckStateRole));
                        }

                        return true;
                    }
                }
                else if (index.internalPointer() == &_universe)
                {
                    if (0 <= index.row() && index.row() < _universe.length())
                    {
                        if (_subset.contains(index.row()) != value.toBool())
                        {
                            if (!value.toBool())
                            {
                                for (int i = 0; i < _subset.length(); i++)
                                {
                                    while (i < _subset.length() && _subset[i] == index.row())
                                    {
                                        beginRemoveRows(this->index(0, 0), i, i);
                                        _subset.removeAt(i);
                                        endRemoveRows();
                                    }
                                }

                                Q_EMIT dataChanged(index, index, QVector<int>(1, Qt::CheckStateRole));
                            }
                            else
                            {
                                beginInsertRows(this->index(0, 0), _subset.length(), _subset.length());
                                _subset += index.row();
                                endInsertRows();

                                Q_EMIT dataChanged(index, index, QVector<int>(1, Qt::CheckStateRole));
                            }
                        }
                    }
                }
            }

            break;
        }
    }

    return false;
}
