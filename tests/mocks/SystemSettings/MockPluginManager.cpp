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

#include "MockPluginManager.h"

#include <QDebug>
#include <QQmlEngine>

MockPluginManager::MockPluginManager(QObject *parent) : QObject(parent)
{
}

QObject* MockPluginManager::getByName(const QString &name) const
{
    qDebug() << "getByName" << name << m_plugins.value(name);
    return m_plugins.value(name);
}

QAbstractItemModel* MockPluginManager::itemModel(const QString &category)
{
    QAbstractItemModel* m = m_models.value(category);
    QQmlEngine::setObjectOwnership(m, QQmlEngine::CppOwnership);
    qDebug() << "itemModel" << category << m;
    return m;
}

void MockPluginManager::resetPlugins()
{
}

QString MockPluginManager::getFilter()
{
    return m_filter;
}

void MockPluginManager::setFilter(const QString &filter)
{
    m_filter = filter;
    Q_EMIT filterChanged();
}

void MockPluginManager::addPlugin(const QString &name, QQmlComponent *entry,
                                  QQmlComponent *page, const QString &category)
{
    MockItemModel *model = nullptr;
    if (m_models.contains(category)) {
        model = (MockItemModel*) m_models.value(category);
    } else {
        model = new MockItemModel(this);
        m_models.insert(category, model);
    }

    MockItem* item = new MockItem(this);
    // item->setProperty("name", name);
    item->setEntryComponent(entry);
    item->setPageComponent(page);
    model->addPlugin(item);
}

bool MockItem::visible()
{
    return true;
}

QQmlComponent* MockItem::entryComponent()
{
    QQmlEngine::setObjectOwnership(m_entry, QQmlEngine::CppOwnership);
    return m_entry;
}
QQmlComponent* MockItem::pageComponent()
{
    QQmlEngine::setObjectOwnership(m_page, QQmlEngine::CppOwnership);
    return m_page;
}

void MockItem::setEntryComponent(QQmlComponent* c)
{
    m_entry = c;
}

void MockItem::setPageComponent(QQmlComponent* c)
{
    m_page = c;
}

MockItemModel::MockItemModel(QObject *parent) : QAbstractListModel(parent)
{
}

void MockItemModel::addPlugin(MockItem *plugin)
{
    int row = m_plugins.size();
    beginInsertRows(QModelIndex(), row, row);
    m_plugins.append(plugin);
    endInsertRows();
}

int MockItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_plugins.count();
}

QVariant MockItemModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= m_plugins.count()) return QVariant();

    const MockItem *item = m_plugins.at(index.row());
    QVariant ret;

    switch (role) {
    case Qt::DisplayRole:
        ret = item->property("name");
        break;
    case IconRole:
        ret = item->property("icon");
        break;
    case ItemRole:
        ret = QVariant::fromValue<QObject*>(const_cast<MockItem*>(item));
        break;
    case KeywordRole:
        ret = QVariant();
    }

    return ret;
}

QHash<int, QByteArray> MockItemModel::roleNames() const
{
    static QHash<int,QByteArray> names;
    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[MockItemModel::IconRole] = "icon";
        names[MockItemModel::ItemRole] = "item";
        names[MockItemModel::KeywordRole] = "keywords";
    }
    return names;
}

// Q_DECLARE_METATYPE(MockItem*)
