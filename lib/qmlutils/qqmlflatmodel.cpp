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

#include "qqmlflatmodel_p_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtQml/qqmlcontext.h>
#include <QtQml/qqmlengine.h>

#include <QtCore/qlist.h>

void QQmlFlatModelPrivate::_q_onRowsInserted(const QModelIndex &index, int first, int last)
{
    Q_Q(QQmlFlatModel);
    if (rootIndex != index) return;
    if (transposed()) {
        // If our row has been moved, reset the model
        if (subRow >= first) {
            q->beginResetModel();
            q->endResetModel();
        }
        return;
    }
    q->beginInsertRows(QModelIndex(), first, last);
    q->endInsertRows();
}

void QQmlFlatModelPrivate::_q_onRowsRemoved(const QModelIndex &index, int first, int last)
{
    Q_Q(QQmlFlatModel);
    if (rootIndex != index) return;
    if (transposed()) {
        // If our row has been deleted or moved, reset the model
        if (subRow >= first) {
            q->beginResetModel();
            q->endResetModel();
        }
        return;
    }
    q->beginRemoveRows(QModelIndex(), first, last);
    q->endRemoveRows();
}

void QQmlFlatModelPrivate::_q_onColumnsInserted(const QModelIndex &index, int first, int last)
{
    Q_Q(QQmlFlatModel);
    if (rootIndex != index) return;
    if (!transposed()) {
        // If our column has been moved, reset the model
        if (subColumn >= first) {
            q->beginResetModel();
            q->endResetModel();
        }
        return;
    }
    q->beginInsertRows(QModelIndex(), first, last);
    q->endInsertRows();
}

void QQmlFlatModelPrivate::_q_onColumnsRemoved(const QModelIndex &index, int first, int last)
{
    Q_Q(QQmlFlatModel);
    if (rootIndex != index) return;
    if (!transposed()) {
        // If our column has been deleted or moved, reset the model
        if (subColumn >= first) {
            q->beginResetModel();
            q->endResetModel();
        }
        return;
    }
    q->beginRemoveRows(QModelIndex(), first, last);
    q->endRemoveRows();
}

void QQmlFlatModelPrivate::_q_onDataChanged(const QModelIndex &topLeft,
                                            const QModelIndex &bottomRight)
{
    Q_Q(QQmlFlatModel);
    if (topLeft.isValid() && bottomRight.isValid()) {
        // Only emit the signal if the changed elements are currently mapped
        if (topLeft.parent() == rootIndex && bottomRight.parent() == rootIndex) {
            if (!transposed()) {
                Q_EMIT q->dataChanged(model->index(topLeft.row(), 0),
                                      model->index(bottomRight.row(), 0));
            } else {
                Q_EMIT q->dataChanged(model->index(topLeft.column(), 0),
                                      model->index(bottomRight.column(), 0));
            }
        }
    } else {
        Q_EMIT q->dataChanged(QModelIndex(), QModelIndex());
    }
}

/*!
    \qmltype FlatModel
    \instantiates QQmlFlatModel
    \inqmlmodule QtQuick 2
    \ingroup qtquick-models
    \brief Map any QAbstractItemModel to a flat list model

    The FlatModel item can be used to represent any QAbstractItemModel as a
    flat list model.  By using the \a subRow and \a subColumn properties one
    can select a particular row or column of a tabular model.  By using the
    enterRow() and goUp() methods, one can navigate through a tree model.
    It can be useful even when proxying a simple QAbstractListModel, because it
    adds some convenience methods such as get() and the \a count property.

    The example below shows how to use FlatModel to navigate through a tree
    model.
    \code
    import QtQuick 2.0

    Item {
        FlatModel {
            id: flatModel
            model: myCppAbstractItemModel
        }

        Rectangle {
            id: backButton
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 50

            Text {
                anchors.fill: parent
                text: "Go up one level"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: flatModel.goUp()
            }
        }

        ListView {
            anchors.top: backButton.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            model: flatModel
            delegate: Rectangle {
                id: cell
                height: 50
                width: 200
                Text {
                    anchors.fill: parent
                    text: name
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: model.enterRow(index)
                }
            }
        }
    }
    \endcode
*/

QQmlFlatModel::QQmlFlatModel(QObject *parent)
    : QAbstractListModel(parent)
    , d_ptr(new QQmlFlatModelPrivate(this))
{
    QObject::connect(this, SIGNAL(rowsInserted(const QModelIndex&,int,int)),
                     this, SIGNAL(countChanged()));
    QObject::connect(this, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
                     this, SIGNAL(countChanged()));
}

QQmlFlatModel::~QQmlFlatModel()
{
    delete d_ptr;
}

/*!
    \qmlproperty QAbstractItemModel QtQuick2::FlatModel::model

    The source model.
*/
void QQmlFlatModel::setModel(QAbstractItemModel *model)
{
    Q_D(QQmlFlatModel);
    if (model == d->model) return;
    beginResetModel();
    if (d->model != 0) {
        d->model->disconnect(this);
    }
    d->model = model;
    d->rootIndex = QModelIndex();
    if (model != 0) {
        QObject::connect(model, SIGNAL(rowsInserted(const QModelIndex&,int,int)),
                         this, SLOT(_q_onRowsInserted(const QModelIndex&,int,int)));
        QObject::connect(model, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
                         this, SLOT(_q_onRowsRemoved(const QModelIndex&,int,int)));
        QObject::connect(model, SIGNAL(columnsInserted(const QModelIndex&,int,int)),
                         this, SLOT(_q_onColumnsInserted(const QModelIndex&,int,int)));
        QObject::connect(model, SIGNAL(columnsRemoved(const QModelIndex&,int,int)),
                         this, SLOT(_q_onColumnsRemoved(const QModelIndex&,int,int)));
        QObject::connect(model, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),
                         this, SLOT(_q_onDataChanged(const QModelIndex&,const QModelIndex&)));
    }
    endResetModel();
    Q_EMIT modelChanged();
}

QAbstractItemModel *QQmlFlatModel::model() const
{
    Q_D(const QQmlFlatModel);
    return d->model;
}

/*!
    \qmlproperty QModelIndex QtQuick2::FlatModel::rootIndex

    This is the parent index of all the items currently listed in the model.
    While this property can be directly written to, it's often easier to modify
    it via the enterRow() and goUp() methods.
*/
void QQmlFlatModel::setRootIndex(const QModelIndex &newRootIndex)
{
    Q_D(QQmlFlatModel);
    if (newRootIndex == d->rootIndex) return;
    beginResetModel();
    d->rootIndex = newRootIndex;
    endResetModel();
    Q_EMIT rootIndexChanged();
}

QModelIndex QQmlFlatModel::rootIndex() const
{
    Q_D(const QQmlFlatModel);
    return d->rootIndex;
}

/*!
    \qmlproperty int QtQuick2::FlatModel::subRow

    If the source model is not a simple list, but rather a table, this property
    allows to choose which of the model's rows should be mapped into the
    FlatModel. If the model is to be read by column, set this to -1.
    By default, this property is -1.
*/
void QQmlFlatModel::setSubRow(int subRow)
{
    Q_D(QQmlFlatModel);
    if (subRow == d->subRow) return;
    beginResetModel();
    d->subRow = subRow;
    endResetModel();
    Q_EMIT subRowChanged();
}

int QQmlFlatModel::subRow() const
{
    Q_D(const QQmlFlatModel);
    return d->subRow;
}

/*!
    \qmlproperty int QtQuick2::FlatModel::subColumn

    If the source model is not a simple list, but rather a table, this property
    allows to choose which of the model's columns should be mapped into the
    FlatModel. If the model is to be read by row, set this to -1.
    By default, this property is 0.
*/
void QQmlFlatModel::setSubColumn(int subColumn)
{
    Q_D(QQmlFlatModel);
    if (subColumn == d->subColumn) return;
    beginResetModel();
    d->subColumn = subColumn;
    endResetModel();
    Q_EMIT subColumnChanged();
}

int QQmlFlatModel::subColumn() const
{
    Q_D(const QQmlFlatModel);
    return d->subColumn;
}

/*
 * \qmlmethod variant QtQuick2::FlatModel::get(int row, string roleName)
 *
 * Returns the data at \a row for the role \a roleName.
 */
QVariant QQmlFlatModel::get(int row, const QString &roleName) const
{
    int role = roleNames().key(roleName.toLatin1(), -1);
    return data(index(row), role);
}

/*
 * \qmlmethod jsobject QtQuick2::FlatModel::get(int row)
 *
 * Returns the data at \a row as an object whose members correspond to the
 * model's roles.
 */
QVariantMap QQmlFlatModel::get(int row) const
{
    QVariantMap map;
    QHash<int,QByteArray> roles = roleNames();
    QModelIndex modelIndex = index(row);
    QHash<int,QByteArray>::const_iterator i = roles.constBegin();
    while (i != roles.constEnd()) {
        map[QString::fromLatin1(i.value())] = data(modelIndex, i.key());
        i++;
    }
    return map;
}

/*
 * \qmlmethod bool QtQuick2::FlatModel::hasChildren(int row)
 *
 * Returns whether the item at \a row has some children.
 */
bool QQmlFlatModel::hasChildren(int row) const
{
    Q_D(const QQmlFlatModel);
    if (Q_UNLIKELY(d->model == 0)) return false;
    return d->model->rowCount(d->mapIndex(row)) > 0;
}

/*
 * \qmlmethod void QtQuick2::FlatModel::enterRow(int row)
 *
 * If the row \a row has children, set it as the root of this model.
 */
void QQmlFlatModel::enterRow(int row)
{
    Q_D(QQmlFlatModel);
    if (Q_UNLIKELY(d->model == 0)) return;
    QModelIndex newRootIndex = d->mapIndex(row);
    setRootIndex(newRootIndex);
}

/*
 * \qmlmethod void QtQuick2::FlatModel::goUp()
 *
 * Move the model up one level.
 */
void QQmlFlatModel::goUp()
{
    Q_D(QQmlFlatModel);
    if (Q_UNLIKELY(d->model == 0)) return;
    QModelIndex newRootIndex = d->rootIndex.parent();
    setRootIndex(newRootIndex);
}

/*
 * \qmlmethod void QtQuick2::FlatModel::modelIndex(int row)
 *
 * Get the \l QModelIndex for the specified index.
 */
QModelIndex QQmlFlatModel::modelIndex(int row) const
{
    Q_D(const QQmlFlatModel);
    if (Q_UNLIKELY(d->model == 0)) return QModelIndex();
    return d->mapIndex(row);
}

/*!
    \qmlproperty int QtQuick2::FlatModel::count

    The number of items in the model.  This property is readonly.
*/
int QQmlFlatModel::rowCount(const QModelIndex &) const
{
    Q_D(const QQmlFlatModel);
    if (Q_UNLIKELY(d->model == 0)) return 0;
    return (d->subColumn >= 0)
        ? d->model->rowCount(d->rootIndex)
        : d->model->columnCount(d->rootIndex);
}

QVariant QQmlFlatModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QQmlFlatModel);
    if (Q_UNLIKELY(d->model == 0)) return QVariant();

    QModelIndex realIndex = d->mapIndex(index.row());
    return realIndex.data(role);
}

QHash<int, QByteArray> QQmlFlatModel::roleNames() const
{
    Q_D(const QQmlFlatModel);
    if (Q_UNLIKELY(d->model == 0)) return QHash<int, QByteArray>();
    return d->model->roleNames();
}

#include <moc_qqmlflatmodel_p.cpp>
