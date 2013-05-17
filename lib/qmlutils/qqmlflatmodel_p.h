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

#ifndef QQMLFLATMODEL_P_H
#define QQMLFLATMODEL_P_H

#include <QtQml/qqml.h>
#include <QtCore/qabstractitemmodel.h>

class QAbstractItemModel;
class QAbstractListModel;

class QQmlFlatModelPrivate;
class QQmlFlatModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel *model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged)
    Q_PROPERTY(int subRow READ subRow WRITE setSubRow NOTIFY subRowChanged)
    Q_PROPERTY(int subColumn READ subColumn WRITE setSubColumn NOTIFY subColumnChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    QQmlFlatModel(QObject *parent = 0);
    virtual ~QQmlFlatModel();

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void setRootIndex(const QModelIndex &newRootIndex);
    QModelIndex rootIndex() const;

    void setSubRow(int subRow);
    int subRow() const;

    void setSubColumn(int subColumn);
    int subColumn() const;

    Q_INVOKABLE QVariant get(int row, const QString &roleName) const;
    Q_INVOKABLE QVariantMap get(int row) const;
    Q_INVOKABLE bool hasChildren(int row) const;
    Q_INVOKABLE void enterRow(int row);
    Q_INVOKABLE void goUp();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QHash<int, QByteArray> roleNames() const;

Q_SIGNALS:
    void modelChanged();
    void rootIndexChanged();
    void subRowChanged();
    void subColumnChanged();
    void countChanged();

private:
    Q_DECLARE_PRIVATE(QQmlFlatModel)
    Q_DISABLE_COPY(QQmlFlatModel)
    Q_PRIVATE_SLOT(d_func(), void _q_onRowsInserted(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_onRowsRemoved(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_onColumnsInserted(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_onColumnsRemoved(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_onDataChanged(const QModelIndex &, const QModelIndex &))
    QQmlFlatModelPrivate *d_ptr;
};

QML_DECLARE_TYPE(QQmlFlatModel)

#endif // QQMLFLATMODEL_P_H
