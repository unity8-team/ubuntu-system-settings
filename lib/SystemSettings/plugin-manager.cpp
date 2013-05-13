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
    QMap<QString,QList<Plugin*> > m_plugins;
    QHash<QString,ItemModel*> m_models;
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
    QMapIterator<QString, QList<Plugin*> > it(m_plugins);
    while (it.hasNext()) {
        it.next();
        Q_FOREACH(Plugin *plugin, it.value()) {
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
        QList<Plugin*> &pluginList = m_plugins[plugin->category()];
        pluginList.append(plugin);
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

QList<Plugin *> PluginManager::plugins(const QString &category) const
{
    Q_D(const PluginManager);
    return d->m_plugins.value(category);
}

QAbstractItemModel *PluginManager::itemModel(const QString &category)
{
    Q_D(PluginManager);
    ItemModel *&model = d->m_models[category];
    if (model == 0) {
        model = new ItemModel(this);
        model->setPlugins(plugins(category));
    }
    return model;
}

void PluginManager::classBegin()
{
    Q_D(PluginManager);
    d->reload();
}

void PluginManager::componentComplete()
{
}
