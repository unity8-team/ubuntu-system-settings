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

#include "plugin-manager.h"
#include "debug.h"
#include "item-model.h"
#include "plugin.h"

#include <QDir>
#include <QMap>
#include <QProcessEnvironment>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QStringList>

using namespace SystemSettings;

static const QLatin1String baseDir{MANIFEST_DIR};

namespace SystemSettings {

class PluginManagerPrivate
{
    Q_DECLARE_PUBLIC(PluginManager)

    inline explicit PluginManagerPrivate(PluginManager *q);
    inline ~PluginManagerPrivate();

    void clear();
    void reload();

private:
    mutable PluginManager *q_ptr;
    QMap<QString, Plugin*> m_plugins;
    ItemModelSortProxy* m_model;
};

} // namespace

PluginManagerPrivate::PluginManagerPrivate(PluginManager *q):
    q_ptr(q),
    m_model(0)
{
}

PluginManagerPrivate::~PluginManagerPrivate()
{
    clear();
}

void PluginManagerPrivate::clear()
{
    Q_FOREACH(Plugin *plugin, m_plugins.values()) {
        delete plugin;
    }
    m_plugins.clear();
}

void PluginManagerPrivate::reload()
{
    Q_Q(PluginManager);
    clear();

    /* Create a list of search paths (e.g. /usr/share, /usr/local/share) and
     * append the baseDir. The reason for not using locateAll is that locateAll
     * does not seem to work with a dir and file pattern, which means it will
     * look for all .settings files, not just those in well-known locations. */
    QStandardPaths::StandardLocation loc = QStandardPaths::GenericDataLocation;
    QFileInfoList searchPaths;
    Q_FOREACH(const QString &path, QStandardPaths::standardLocations(loc)) {
        QDir dir(QStringLiteral("%1/%2").arg(path, baseDir), "*.settings");
        searchPaths.append(dir.entryInfoList());
    }

    /* Use an environment variable USS_SHOW_ALL_UI to show unfinished / beta /
     * deferred components or panels */
    bool showAll = false;
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    if (environment.contains(QLatin1String("USS_SHOW_ALL_UI"))) {
        QString showAllS = environment.value("USS_SHOW_ALL_UI", QString());
        showAll = !showAllS.isEmpty();
    }

    QQmlContext *ctx = QQmlEngine::contextForObject(q);
    if (ctx)
        ctx->engine()->rootContext()->setContextProperty("showAllUI", showAll);

    Q_FOREACH(const QFileInfo &fileInfo, searchPaths) {
        Plugin *plugin = new Plugin(fileInfo);
        QQmlEngine::setContextForObject(plugin, ctx);
        if (showAll || !plugin->hideByDefault())
            m_plugins.insert(fileInfo.baseName(), plugin);
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

QMap<QString, Plugin *> PluginManager::plugins() const
{
    Q_D(const PluginManager);
    return d->m_plugins;
}

void PluginManager::resetPlugins()
{
    Q_D(const PluginManager);
    Q_FOREACH (Plugin *plugin, d->m_plugins) {
            plugin->reset();
    }
}

QAbstractItemModel *PluginManager::itemModel()
{
    Q_D(PluginManager);
    if (d->m_model == 0) {
        ItemModel *backing_model = new ItemModel(this);
        backing_model->setPlugins(plugins());
        /* Return a sorted proxy backed by the real model containing the items */
        d->m_model = new ItemModelSortProxy(this);
        d->m_model->setSourceModel(backing_model);
        d->m_model->setDynamicSortFilter(true);
        d->m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
        d->m_model->setFilterRole(ItemModel::KeywordRole);
        /* we only have one column as this is a QAbstractListModel */
        d->m_model->sort(0);
    }

    return d->m_model;
}

QObject *PluginManager::getByName(const QString &name) const
{
    Q_D(const PluginManager);
    if (d->m_plugins.contains(name))
        return d->m_plugins[name];

    return nullptr;
}

QString PluginManager::getFilter()
{
    return m_filter;
}

void PluginManager::setFilter(const QString &filter)
{
    Q_D(PluginManager);
    if (filter.isEmpty())
        d->m_model->setFilterRegExp("");
    else
        d->m_model->setFilterRegExp(filter);
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
