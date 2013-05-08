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

#include <QStringList>

using namespace SystemSettings;

namespace SystemSettings {

const QLatin1String keyName("name");
const QLatin1String keyIcon("icon");
const QLatin1String keyCategory("category");
const QLatin1String keyPriority("priority");
const QLatin1String keyTranslations("translations");
const QLatin1String keyFormFactors("form-factors");
const QLatin1String keyKeywords("keywords");
const QLatin1String keyPlugin("plugin");
const QLatin1String keyHasDynamicKeywords("has-dynamic-keywords");
const QLatin1String keyHasDynamicVisibility("has-dynamic-visibility");

class PluginBasePrivate
{
    friend class PluginBase;

    inline PluginBasePrivate(const QVariantMap &staticData);
    ~PluginBasePrivate() {};

private:
    QVariantMap m_data;
    QUrl m_icon;
    QStringList m_keywords;
    bool m_isVisible;
};

} // namespace

PluginBasePrivate::PluginBasePrivate(const QVariantMap &staticData):
    m_data(staticData),
    m_isVisible(false)
{
}

PluginBase::PluginBase(const QVariantMap &staticData, QObject *parent):
    QObject(parent),
    d_ptr(new PluginBasePrivate(staticData))
{
}

PluginBase::~PluginBase()
{
    delete d_ptr;
}

void PluginBase::setIcon(const QUrl &icon)
{
    Q_D(PluginBase);
    if (icon == d->m_icon) return;
    d->m_icon = icon;
    Q_EMIT iconChanged();
}

QUrl PluginBase::icon() const
{
    Q_D(const PluginBase);
    return d->m_icon;
}

void PluginBase::setKeywords(const QStringList &keywords)
{
    Q_D(PluginBase);
    if (keywords == d->m_keywords) return;
    d->m_keywords = keywords;
    Q_EMIT keywordsChanged();
}

QStringList PluginBase::keywords() const
{
    Q_D(const PluginBase);
    return d->m_keywords;
}

void PluginBase::setVisible(bool visible)
{
    Q_D(PluginBase);
    if (visible == d->m_isVisible) return;
    d->m_isVisible = visible;
    Q_EMIT visibilityChanged();
}

bool PluginBase::isVisible() const
{
    Q_D(const PluginBase);
    return d->m_isVisible;
}

const QVariantMap &PluginBase::staticData() const
{
    Q_D(const PluginBase);
    return d->m_data;
}

QQmlComponent *PluginBase::entryComponent(QQmlEngine *engine, QObject *parent)
{
    Q_UNUSED(engine);
    Q_UNUSED(parent);
    return 0;
}

QQmlComponent *PluginBase::pageComponent(QQmlEngine *engine, QObject *parent)
{
    Q_UNUSED(engine);
    Q_UNUSED(parent);
    return 0;
}
