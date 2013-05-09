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
#include "plugin-base.h"
#include "plugin-interface.h"
#include "plugin.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QPluginLoader>
#include <QStringList>
#include <QVariantMap>

using namespace SystemSettings;

static const QLatin1String pluginModuleDir(PLUGIN_MODULE_DIR);

namespace SystemSettings {

class PluginPrivate
{
    friend class Plugin;

    inline PluginPrivate(const QFileInfo &manifest);
    ~PluginPrivate() {};

    bool ensureLoaded() const;

private:
    mutable PluginBase *m_plugin;
    mutable QPluginLoader m_loader;
    QVariantMap m_data;
};

} // namespace

PluginPrivate::PluginPrivate(const QFileInfo &manifest):
    m_plugin(0)
{
    QFile file(manifest.filePath());
    if (Q_UNLIKELY(!file.open(QIODevice::ReadOnly | QIODevice::Text))) {
        qWarning() << "Couldn't open file" << manifest.filePath();
        return;
    }

    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &error);
    if (Q_UNLIKELY(json.isEmpty())) {
        qWarning() << "File is empty:" << manifest.filePath() <<
            error.errorString();
        return;
    }

    m_data = json.toVariant().toMap();
}

bool PluginPrivate::ensureLoaded() const
{
    if (m_plugin != 0) return true;

    if (Q_UNLIKELY(m_loader.isLoaded())) return false;
    QString name = QString("%1/lib%2.so").arg(pluginModuleDir).
        arg(m_data.value(keyPlugin).toString());

    m_loader.setFileName(name);
    if (Q_UNLIKELY(!m_loader.load())) {
        qWarning() << m_loader.errorString() << name;
        return false;
    }

    PluginInterface *interface =
        qobject_cast<SystemSettings::PluginInterface*>(m_loader.instance());
    if (Q_UNLIKELY(interface == 0)) {
        qWarning() << name << "doesn't implement PluginInterface";
        return false;
    }

    m_plugin = interface->createPlugin(m_data);
    return m_plugin != 0;
}

Plugin::Plugin(const QFileInfo &manifest, QObject *parent):
    QObject(parent),
    d_ptr(new PluginPrivate(manifest))
{
}

Plugin::~Plugin()
{
    delete d_ptr;
}

QString Plugin::displayName() const
{
    Q_D(const Plugin);
    return d->m_data.value(keyName).toString();
}

QUrl Plugin::icon() const
{
    Q_D(const Plugin);
    QString iconName = d->m_data.value(keyIcon).toString();
    if (iconName.isEmpty()) {
        if (!d->ensureLoaded()) return QUrl();
        return d->m_plugin->icon();
    } else {
        return QString("image://gicon/") + QUrl::toPercentEncoding(iconName);
    }
}

QString Plugin::category() const
{
    Q_D(const Plugin);
    return d->m_data.value(keyCategory).toString();
}

int Plugin::priority() const
{
    Q_D(const Plugin);
    return d->m_data.value(keyPriority).toInt();
}

QString Plugin::translations() const
{
    Q_D(const Plugin);
    return d->m_data.value(keyTranslations).toString();
}

QStringList Plugin::keywords() const
{
    Q_D(const Plugin);
    QStringList ret = d->m_data.value(keyKeywords).toStringList();
    if (d->m_data.value(keyHasDynamicKeywords).toBool()) {
        if (!d->ensureLoaded()) return ret;
        ret += d->m_plugin->keywords();
    }
    return ret;
}

bool Plugin::isVisible() const
{
    Q_D(const Plugin);
    // TODO: visibility check depending on form-factor
    if (d->m_data.value(keyHasDynamicVisibility).toBool()) {
        if (!d->ensureLoaded()) return false;
        return d->m_plugin->isVisible();
    }
    return true;
}
