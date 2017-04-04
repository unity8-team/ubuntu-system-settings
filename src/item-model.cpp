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

#include "item-model.h"

#include <glib.h>
#include <libintl.h>

#include "debug.h"
#include "plugin.h"

using namespace SystemSettings;

namespace SystemSettings {

class ItemModelPrivate
{
    friend class ItemModel;

    inline ItemModelPrivate();
    inline ~ItemModelPrivate();

private:
    QHash<int, QByteArray> m_roleNames;
    QMap<QString, Plugin *> m_plugins;
    QList<Plugin *> m_visibleItems;
};

} // namespace

ItemModelPrivate::ItemModelPrivate()
{
    m_roleNames[Qt::DisplayRole] = "displayName";
    m_roleNames[ItemModel::IconRole] = "icon";
    m_roleNames[ItemModel::ItemRole] = "item";
    m_roleNames[ItemModel::KeywordRole] = "keywords";
}

ItemModelPrivate::~ItemModelPrivate()
{
}

ItemModel::ItemModel(QObject *parent):
    QAbstractListModel(parent),
    d_ptr(new ItemModelPrivate)
{
}

ItemModel::~ItemModel()
{
    delete d_ptr;
}

void ItemModel::setPlugins(const QMap<QString, Plugin *> &plugins)
{
    Q_D(ItemModel);
    beginResetModel();
    d->m_plugins = plugins;
    Q_FOREACH(Plugin *plugin, d->m_plugins.values()) {
        QObject::connect(plugin, SIGNAL(visibilityChanged()),
                         this, SLOT(onItemVisibilityChanged()));
        d->m_visibleItems.append(plugin);
    }
    endResetModel();
}

int ItemModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const ItemModel);
    Q_UNUSED(parent);
    return d->m_visibleItems.count();
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
    Q_D(const ItemModel);

    if (index.row() >= d->m_visibleItems.count()) return QVariant();

    const Plugin *item = d->m_visibleItems.at(index.row());
    QVariant ret;

    switch (role) {
    case Qt::DisplayRole:
        ret = item->displayName();
        break;
    case IconRole:
        ret = item->icon();
        break;
    case ItemRole:
        ret = QVariant::fromValue<QObject*>(const_cast<Plugin*>(item));
        break;
    case KeywordRole:
        QByteArray translations = item->translations().toUtf8();
        QByteArray displayName = item->displayName().toUtf8();
        const char * domain = translations.constData();
        QStringList temp(item->keywords());
        QMutableListIterator<QString> it(temp);
        while (it.hasNext()) {
            QString keyword = it.next();
            it.setValue(QString::fromUtf8(
                            dgettext(
                                domain,
                                keyword.toUtf8().constData())));
        }
        temp << QString::fromUtf8(
                    dgettext(domain,displayName.constData()));
        ret = temp;
    }

    return ret;
}

QHash<int, QByteArray> ItemModel::roleNames() const
{
    Q_D(const ItemModel);
    return d->m_roleNames;
}

int ItemModel::getIndexByName(const QString& name) const
{
    Q_D(const ItemModel);
    for (int i = 0; i < d->m_visibleItems.count(); ++i) {
        if (d->m_visibleItems.at(i)->baseName() == name) {
            return i;
        }
    }

    return -1;
}

QString ItemModelSortProxy::getNameByIndex(int row) const
{
    if (row < 0 || row >= rowCount()) {
        return QString();
    }

    QVariant data = index(row, 0).data(ItemModel::ItemRole);
    Plugin *plugin = data.value<Plugin *>();

    if (plugin != NULL) {
        return plugin->baseName();
    }

    return QString();
}

int ItemModelSortProxy::getPreviousVisibleIndex(int from) const
{
    int idx = from - 1;

    while (idx != from) {
        if (idx < 0) {
            idx = rowCount() - 1;
        }

        QVariant data = index(idx, 0).data(ItemModel::ItemRole);
        Plugin *plugin = data.value<Plugin *>();
        if (plugin && plugin->isVisible()) {
            return idx;
        }

        idx--;
    }

    return from;
}

int ItemModelSortProxy::getNextVisibleIndex(int from) const
{
    int idx = from + 1;

    while (idx != from) {
        if (idx == rowCount()) {
            idx = 0;
        }

        QVariant data = index(idx, 0).data(ItemModel::ItemRole);
        Plugin *plugin = data.value<Plugin *>();
        if (plugin && plugin->isVisible()) {
            return idx;
        }

        idx++;
    }

    return from;
}

void ItemModel::onItemVisibilityChanged()
{
    Q_D(ItemModel);

    Plugin *item = qobject_cast<Plugin *>(sender());
    Q_ASSERT(item != 0);

    QModelIndex root;
    int index = d->m_visibleItems.indexOf(item);
    if (item->isVisible()) {
        if (index >= 0) return; // nothing to do
        index = d->m_visibleItems.count();
        beginInsertRows(root, index, index);
        d->m_visibleItems.append(item);
        endInsertRows();
    } else {
        if (index < 0) return; // nothing to do
        beginRemoveRows(root, index, index);
        d->m_visibleItems.removeAt(index);
        endRemoveRows();
    }
}

ItemModelSortProxy::ItemModelSortProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_categoriesOrder << "uncategorized-top";
    m_categoriesOrder << "network";
    m_categoriesOrder << "personal";
    m_categoriesOrder << "system";
    m_categoriesOrder << "uncategorized-bottom";
}

int ItemModelSortProxy::getIndexByName(const QString& name) const
{
   ItemModel *source = (ItemModel*) sourceModel();
   int sourceRow = source->getIndexByName(name);

   if (sourceRow < 0) {
       return sourceRow;
   }

   return mapFromSource(source->index(sourceRow, 0)).row();
}

bool ItemModelSortProxy::lessThan(const QModelIndex &left,
                                  const QModelIndex &right) const
{
    QVariant leftData(sourceModel()->data(left, ItemModel::ItemRole));
    QVariant rightData(sourceModel()->data(right, ItemModel::ItemRole));

    Plugin *leftPlugin = leftData.value<Plugin *>();
    Plugin *rightPlugin = rightData.value<Plugin *>();

    if (leftPlugin && rightPlugin) {
        int leftCategoryOrder = m_categoriesOrder.indexOf(leftPlugin->category());
        int rightCategoryOrder = m_categoriesOrder.indexOf(rightPlugin->category());

        if (leftCategoryOrder == rightCategoryOrder) {
            int leftPriority = leftPlugin->priority();
            int rightPriority = rightPlugin->priority();

            /* In case two plugins happen to have the same priority, sort them
               alphabetically */
            if (leftPriority == rightPriority)
                return leftPlugin->displayName() < rightPlugin->displayName();

            return leftPriority < rightPriority;
        }

        return leftCategoryOrder < rightCategoryOrder;
    }

    return false;
}

bool ItemModelSortProxy::filterAcceptsRow(
        int source_row, const QModelIndex &source_parent) const
{
    QStringList keywords;
    gchar * pattern = nullptr;
    bool ret = false;
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    QVariant data(sourceModel()->data(index, filterRole()));
    pattern = g_strdup(filterRegExp().pattern().toStdString().c_str());

    switch (filterRole()) {
    case ItemModel::KeywordRole:
        keywords = data.value<QStringList>();

        foreach (const QString& s, keywords)
            if (g_str_match_string (pattern, s.toStdString().c_str(), TRUE)) {
                ret = true;
                goto out;
            }
    default:
        ret = false;
    }

out:
    g_free (pattern);
    return ret;
}
