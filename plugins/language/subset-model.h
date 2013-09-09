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

class SubsetModel : public QAbstractListModel
{
protected:

    Q_OBJECT

public:

    Q_PROPERTY(QStringList superset
               READ superset
               WRITE setSuperset
               NOTIFY supersetChanged)

    Q_PROPERTY(QList<int> subset
               READ subset
               WRITE setSubset
               NOTIFY subsetChanged)

    explicit SubsetModel(QObject *parent = NULL);

    virtual const QStringList &superset() const;
    virtual void setSuperset(const QStringList &superset);
    Q_SIGNAL virtual void supersetChanged() const;

    virtual const QList<int> &subset() const;
    virtual void setSubset(const QList<int> &subset);
    Q_SIGNAL virtual void subsetChanged() const;

    Q_INVOKABLE virtual void setChecked(int  element,
                                        bool checked,
                                        int  timeout);

    virtual QHash<int, QByteArray> roleNames() const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index,
                          int                role  = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index,
                         const QVariant    &value,
                         int                role  = Qt::EditRole);

protected:

    Q_SLOT virtual void timerExpired();

    virtual int elementAtRow(int row) const;
    virtual int elementAtIndex(const QModelIndex &index) const;

    struct State {
        bool checked;
        qint64 check;
        qint64 uncheck;
    };

    struct Change {
        int element;
        bool checked;
        qint64 start;
        qint64 finish;
    };

    QStringList _superset;
    QList<int> _subset;

    QList<State *> _state;
    QList<Change *> _change;

    qint64 _ignore;

    friend bool changeLessThan(const Change *change0,
                               const Change *change1);
};

#endif // SUBSET_MODEL_H
