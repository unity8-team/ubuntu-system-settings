/*
 * This file is part of system-settings
 *
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

#ifndef SYSTEM_SETTINGS_ITEM_MODEL_H
#define SYSTEM_SETTINGS_ITEM_MODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

namespace SystemSettings {

class Plugin;

class ItemModelPrivate;
class ItemModel: public QAbstractListModel
{
    Q_OBJECT

public:
    ItemModel(QObject *parent = 0);
    ~ItemModel();

    enum Roles {
        IconRole = Qt::UserRole + 1,
        ItemRole,
        KeywordRole,
    };
    void setPlugins(const QMap<QString, Plugin *> &plugins);

    // reimplemented virtual methods
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

private Q_SLOTS:
    void onItemVisibilityChanged();

private:
    ItemModelPrivate *d_ptr;
    Q_DECLARE_PRIVATE(ItemModel)
};

class ItemModelSortProxy: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ItemModelSortProxy(QObject *parent = 0);

protected:
    virtual bool lessThan(const QModelIndex &left,
                          const QModelIndex &right) const;
    virtual bool filterAcceptsRow(int source_row,
                                  const QModelIndex &source_parent) const;
};

} // namespace

#endif // SYSTEM_SETTINGS_ITEM_MODEL_H
