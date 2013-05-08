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
#include "plugin.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QStringList>
#include <QVariantMap>

using namespace SystemSettings;

static const QLatin1String keyName("name");
static const QLatin1String keyIcon("icon");
static const QLatin1String keyCategory("category");
static const QLatin1String keyPriority("priority");
static const QLatin1String keyTranslations("translations");
static const QLatin1String keyFormFactors("form-factors");
static const QLatin1String keyKeywords("keywords");
static const QLatin1String keyPlugin("plugin");
static const QLatin1String keyHasDynamicKeywords("has-dynamic-keywords");
static const QLatin1String keyHasDynamicVisibility("has-dynamic-visibility");

namespace SystemSettings {

class PluginPrivate
{
    friend class Plugin;

    inline PluginPrivate(const QFileInfo &manifest);
    ~PluginPrivate() {};

private:
    QVariantMap m_data;
};

} // namespace

PluginPrivate::PluginPrivate(const QFileInfo &manifest)
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
        // TODO: load the plugin and get the icon from there
        return QUrl();
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
        // TODO: load the plugin and query run-time keywords
    }
    return ret;
}

bool Plugin::isVisible() const
{
    // TODO: visibility check depending on form-factor
    // TODO: run-time visibility check if keyHasDynamicVisibility is true
    return true;
}
