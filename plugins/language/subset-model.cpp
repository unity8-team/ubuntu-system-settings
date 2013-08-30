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

#define SECTION_ROLE (Qt::UserRole + 0)

SubsetModel::SubsetModel(const QStringList &superset,
                         QObject           *parent) :
    QAbstractListModel(parent),
    _superset(superset)
{
}

const QStringList &
SubsetModel::superset() const
{
    return _superset;
}

const QList<int> &
SubsetModel::subset() const
{
    return _subset;
}

void
SubsetModel::setSubset(const QList<int> &subset)
{
    if (subset != _subset)
    {
        beginResetModel();
        _subset = subset;
        endResetModel();

        Q_EMIT subsetChanged();
    }
}

bool
SubsetModel::setInSubset(int  element,
                         bool inSubset)
{
    if (inSubset != _subset.contains(element))
    {
        if (inSubset)
        {
            beginInsertRows(QModelIndex(), _subset.length(), _subset.length());
            _subset += element;
            endInsertRows();
        }
        else
        {
            for (int i = 0; i < _subset.length(); i++)
            {
                while (i < _subset.length() && _subset[i] == element)
                {
                    beginRemoveRows(QModelIndex(), i, i);
                    _subset.removeAt(i);
                    endRemoveRows();
                }
            }
        }

        QModelIndex changed(index(_subset.length() + element, 0));
        Q_EMIT dataChanged(changed, changed, QVector<int>(1, Qt::CheckStateRole));
        Q_EMIT subsetChanged();

        return true;
    }

    return false;
}

QHash<int, QByteArray>
SubsetModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;

    roleNames.insert(SECTION_ROLE, "section");
    roleNames.insert(Qt::DisplayRole, "label");
    roleNames.insert(Qt::CheckStateRole, "checked");

    return roleNames;
}

int
SubsetModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _subset.length() + _superset.length();
}

Qt::ItemFlags
SubsetModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);

    return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
}

QVariant
SubsetModel::data(const QModelIndex &index,
                  int                role) const
{
    switch (role)
    {
    case SECTION_ROLE:
        return index.row() < _subset.length() ? "subset" : "superset";

    case Qt::DisplayRole:
        return index.row() < _subset.length() ? _superset[_subset[index.row()]] : _superset[index.row() - _subset.length()];

    case Qt::CheckStateRole:
        return index.row() < _subset.length() ? true : _subset.contains(index.row() - _subset.length());
    }

    return QVariant();
}

bool
SubsetModel::setData(const QModelIndex &index,
                     const QVariant    &value,
                     int                role)
{
    switch (role)
    {
    case Qt::CheckStateRole:
        if (static_cast<QMetaType::Type>(value.type()) == QMetaType::Bool)
        {
            if (index.row() < _subset.length())
            {
                if (setInSubset(_subset[index.row()], false))
                    return true;
            }
            else
            {
                if (setInSubset(index.row() - _subset.length(), value.toBool()))
                    return true;
            }
        }

        break;
    }

    return false;
}
