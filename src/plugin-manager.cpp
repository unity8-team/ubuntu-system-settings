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

#include "debug.h"
#include "item-model.h"
#include "plugin.h"
#include "plugin-manager.h"

#include <QDir>
#include <QMap>
#include <QQmlEngine>
#include <QStringList>

using namespace SystemSettings;

static const QLatin1String baseDir(PLUGIN_MANIFEST_DIR);

namespace SystemSettings {

class PluginManagerPrivate
{
    Q_DECLARE_PUBLIC(PluginManager)

    inline PluginManagerPrivate(PluginManager *q);
    inline ~PluginManagerPrivate();

    void clear();
    void reload();

private:
    mutable PluginManager *q_ptr;
    QMap<QString,QMap<QString, Plugin*> > m_plugins;
    QHash<QString,ItemModelSortProxy*> m_models;
};

} // namespace

PluginManagerPrivate::PluginManagerPrivate(PluginManager *q):
    q_ptr(q)
{
}

PluginManagerPrivate::~PluginManagerPrivate()
{
    clear();
}

void PluginManagerPrivate::clear()
{
    QMapIterator<QString, QMap<QString, Plugin*> > it(m_plugins);
    while (it.hasNext()) {
        it.next();
        Q_FOREACH(Plugin *plugin, it.value().values()) {
            delete plugin;
        }
    }
    m_plugins.clear();
}

void PluginManagerPrivate::reload()
{
    Q_Q(PluginManager);
    clear();
    QDir path(baseDir, "*.settings");
    Q_FOREACH(QFileInfo fileInfo, path.entryInfoList()) {
        Plugin *plugin = new Plugin(fileInfo);
        QQmlEngine::setContextForObject(plugin,
                                        QQmlEngine::contextForObject(q));
        QMap<QString, Plugin*> &pluginList = m_plugins[plugin->category()];
        pluginList.insert(fileInfo.baseName(), plugin);
    }
}

PluginManager::PluginManager(QObject *parent):
    QObject(parent),
    d_ptr(new PluginManagerPrivate(this))
{
}

PluginManager::~PluginManager()
{
    delete d_ptr;
}

QStringList PluginManager::categories() const
{
    Q_D(const PluginManager);
    return d->m_plugins.keys();
}

QMap<QString, Plugin *> PluginManager::plugins(const QString &category) const
{
    Q_D(const PluginManager);
    return d->m_plugins.value(category);
}

QAbstractItemModel *PluginManager::itemModel(const QString &category)
{
    Q_D(PluginManager);
    ItemModelSortProxy *&model = d->m_models[category];
    if (model == 0) {
        ItemModel *backing_model = new ItemModel(this);
        backing_model->setPlugins(plugins(category));
        /* Return a sorted proxy backed by the real model containing the items */
        model = new ItemModelSortProxy(this);
        model->setSourceModel(backing_model);
        model->setDynamicSortFilter(true);
        model->setFilterCaseSensitivity(Qt::CaseInsensitive);
        model->setFilterRole(ItemModel::KeywordRole);
        /* we only have one column as this is a QAbstractListModel */
        model->sort(0);
    }

    return model;
}

QObject *PluginManager::getByName(const QString &name) const
{
    Q_D(const PluginManager);
    QMapIterator<QString, QMap<QString, Plugin *> > plugins(d->m_plugins);
    while (plugins.hasNext()) {
        plugins.next();
        if (plugins.value().contains(name))
            return plugins.value()[name];
    }
    return NULL;
}

QString PluginManager::getFilter()
{
    return m_filter;
}

void PluginManager::setFilter(const QString &filter)
{
    Q_D(PluginManager);
    QHashIterator<QString,ItemModelSortProxy*> it(d->m_models);
    while (it.hasNext()) {
        it.next();
        if (filter.isEmpty())
            it.value()->setFilterRegExp("");
        else
            it.value()->setFilterRegExp(filter);
    }
    m_filter = filter;
    Q_EMIT (filterChanged());
}

void PluginManager::classBegin()
{
    Q_D(PluginManager);
    d->reload();
}

void PluginManager::componentComplete()
{
}
