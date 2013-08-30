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

#ifndef SUBSET_MODEL_H
#define SUBSET_MODEL_H

#include <QtCore>

class SubsetModel : public QAbstractItemModel
{
private:

    Q_OBJECT

    Q_PROPERTY(QStringList universe
               READ universe
               CONSTANT)

    Q_PROPERTY(QList<int> subset
               READ subset
               WRITE setSubset
               NOTIFY subsetChanged)

public:

    explicit SubsetModel(const QStringList &universe = QStringList(), QObject *parent = NULL);

    const QStringList &universe() const;

    const QList<int> &subset() const;
    void setSubset(const QList<int> &list);
    Q_SIGNAL void subsetChanged() const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;

    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QHash<int, QByteArray> roleNames() const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

private:

    QStringList _universe;
    QList<int> _subset;
};

#endif // SUBSET_MODEL_H
