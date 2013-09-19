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

#ifndef SYSTEM_SETTINGS_PLUGIN_H
#define SYSTEM_SETTINGS_PLUGIN_H

#include <QObject>
#include <QQmlComponent>
#include <QStringList>
#include <QUrl>

class QFileInfo;

namespace SystemSettings {

class PluginPrivate;
class Plugin: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *entryComponent READ entryComponent CONSTANT)
    Q_PROPERTY(QQmlComponent *pageComponent READ pageComponent CONSTANT)
    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(QUrl icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(QString category READ category CONSTANT)
    Q_PROPERTY(int priority READ priority CONSTANT)
    Q_PROPERTY(QString translations READ translations CONSTANT)
    Q_PROPERTY(QStringList keywords READ keywords NOTIFY keywordsChanged)
    Q_PROPERTY(bool visible READ isVisible NOTIFY visibilityChanged)
    Q_PROPERTY(bool hideByDefault READ hideByDefault CONSTANT)

public:
    Plugin(const QFileInfo &manifest, QObject *parent = 0);
    ~Plugin();

    QString displayName() const;
    QUrl icon() const;
    QString category() const;
    int priority() const;
    QString translations() const;
    QStringList keywords() const;
    bool isVisible() const;
    bool hideByDefault() const;

    QQmlComponent *entryComponent();
    QQmlComponent *pageComponent();

Q_SIGNALS:
    void iconChanged();
    void keywordsChanged();
    void visibilityChanged();

private:
    PluginPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Plugin)
};

} // namespace

#endif // SYSTEM_SETTINGS_PLUGIN_H
