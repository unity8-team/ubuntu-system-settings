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

#ifndef SYSTEM_SETTINGS_PLUGIN_BASE_H
#define SYSTEM_SETTINGS_PLUGIN_BASE_H

#include <QObject>
#include <QQmlComponent>
#include <QVariantMap>

namespace SystemSettings {

extern const QLatin1String keyName;
extern const QLatin1String keyIcon;
extern const QLatin1String keyCategory;
extern const QLatin1String keyPriority;
extern const QLatin1String keyTranslations;
extern const QLatin1String keyFormFactors;
extern const QLatin1String keyKeywords;
extern const QLatin1String keyPluginBase;
extern const QLatin1String keyHasDynamicKeywords;
extern const QLatin1String keyHasDynamicVisibility;

class PluginBasePrivate;
class PluginBase: public QObject
{
    Q_OBJECT

public:
    PluginBase(const QVariantMap &staticData, QObject *parent = 0);
    ~PluginBase();

    QUrl icon() const;
    QStringList keywords() const;
    bool isVisible() const;
    virtual QQmlComponent *entryComponent(QQmlEngine *engine,
                                          QObject *parent = 0);
    virtual QQmlComponent *pageComponent(QQmlEngine *engine,
                                         QObject *parent = 0);

protected:
    void setIcon(const QUrl &icon);
    void setKeywords(const QStringList &keywords);
    void setVisible(bool visible);
    const QVariantMap &staticData() const;

Q_SIGNALS:
    void iconChanged();
    void keywordsChanged();
    void visibilityChanged();

private:
    PluginBasePrivate *d_ptr;
    Q_DECLARE_PRIVATE(PluginBase)
};

} // namespace

#endif // SYSTEM_SETTINGS_PLUGIN_BASE_H
