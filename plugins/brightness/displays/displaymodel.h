/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#ifndef DISPLAY_MODEL_H
#define DISPLAY_MODEL_H

#include "display.h"

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QVariant>

namespace DisplayPlugin
{
class DisplayModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    explicit DisplayModel(QObject *parent = 0);
    ~DisplayModel();

    enum Roles
    {
        // Qt::DisplayRole holds device name
        TypeRole = Qt::UserRole,
        MirroredRole,
        ConnectedRole,
        EnabledRole,
        ModeRole,
        ModesRole,
        OrientationRole,
        ScaleRole,
        UncommittedChangesRole,
        LastRole = UncommittedChangesRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
    QHash<int,QByteArray> roleNames() const;
    void addDisplay(const QSharedPointer<Display> &display);
    QSharedPointer<Display> getById(const uint &id);
    int findRowFromId(const uint &id);

Q_SIGNALS:
    void countChanged();

protected:
    void emitRowChanged(const int &row);

private:
    QList<QSharedPointer<Display> > m_displays;

private slots:
    void displayChangedSlot(const Display *display);
};

class DisplaysFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    DisplaysFilter();
    virtual ~DisplaysFilter() {}

    void filterOnUncommittedChanges(const bool uncommitted);
    void filterOnConnected(const bool connected);

Q_SIGNALS:
    void countChanged();

protected:
    virtual bool filterAcceptsRow(int, const QModelIndex&) const;
    virtual bool lessThan(const QModelIndex&, const QModelIndex&) const;

private:
    bool m_uncommittedChanges = false;
    bool m_uncommittedChangesEnabled = false;

    bool m_connected = false;
    bool m_connectedEnabled = false;

private slots:
    void rowsChanged(const QModelIndex &parent, int first, int last);
};
} // DisplayPlugin

#endif // DISPLAY_MODEL_H
