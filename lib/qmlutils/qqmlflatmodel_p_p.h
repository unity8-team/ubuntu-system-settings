/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#ifndef QQMLFLATMODEL_P_P_H
#define QQMLFLATMODEL_P_P_H

#include "qqmlflatmodel_p.h"

#include <QtCore/qpointer.h>

class QQmlFlatModelPrivate
{
    Q_DECLARE_PUBLIC(QQmlFlatModel)

public:
    QQmlFlatModelPrivate() : model(0)
        , subRow(-1), subColumn(0) {}
    void _q_onRowsInserted(const QModelIndex &index, int first, int last);
    void _q_onRowsRemoved(const QModelIndex &index, int first, int last);
    void _q_onColumnsInserted(const QModelIndex &index, int first, int last);
    void _q_onColumnsRemoved(const QModelIndex &index, int first, int last);
    void _q_onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    QModelIndex mapIndex(int row) const {
        return model->index((subRow >= 0) ? subRow : row,
                            (subColumn >= 0) ? subColumn : row,
                            rootIndex);
    }

    bool transposed() { return subRow >= 0; }

    QQmlFlatModel *q_ptr;
    QPointer<QAbstractItemModel> model;
    QPersistentModelIndex rootIndex;
    int subRow;
    int subColumn;
};

#endif // QQMLFLATMODEL_P_P_H
