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

MockPluginManager::MockPluginManager(QObject *parent) :
    QObject(parent),
    m_model(0)
{
}

QObject* MockPluginManager::getByName(const QString &name) const
{
    MockItem* p = m_plugins.value(name);
    QQmlEngine::setObjectOwnership(p, QQmlEngine::CppOwnership);
    return p;
}

QAbstractItemModel* MockPluginManager::itemModel()
{
    QAbstractItemModel* m = m_model;
    QQmlEngine::setObjectOwnership(m, QQmlEngine::CppOwnership);
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
                                  QQmlComponent *page)
{
    if (!m_model) {
        m_model = new MockItemModel(this);
    }

    MockItem* item = new MockItem(this);
    item->setBaseName(name);
    item->setEntryComponent(entry);
    item->setPageComponent(page);
    m_model->addPlugin(item);
    m_plugins.insert(name, item);
}

bool MockItem::visible() const
{
    return true;
}

QString MockItem::baseName() const
{
    return m_baseName;
}

void MockItem::setBaseName(const QString &baseName)
{
    m_baseName = baseName;
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
        ret = item->baseName();
        break;
    case ItemRole:
        ret = QVariant::fromValue<QObject*>(const_cast<MockItem*>(item));
        break;
    }

    return ret;
}

QHash<int, QByteArray> MockItemModel::roleNames() const
{
    static QHash<int,QByteArray> names;
    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[MockItemModel::ItemRole] = "item";
    }
    return names;
}

int MockItemModel::getIndexByName(const QString& name) const
{
    for (int i = 0; i < m_plugins.count(); ++i) {
        if (m_plugins.at(i)->baseName() == name) {
            return i;
        }
    }

    return -1;
}

// Q_DECLARE_METATYPE(MockItem*)
