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

#ifndef SYSTEM_SETTINGS_PLUGIN_MANAGER_H
#define SYSTEM_SETTINGS_PLUGIN_MANAGER_H

#include <QList>
#include <QObject>
#include <QQmlParserStatus>

class QAbstractItemModel;

namespace SystemSettings {

class Plugin;

class PluginManagerPrivate;
class PluginManager: public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY (QString filter
                READ getFilter
                WRITE setFilter
                NOTIFY filterChanged)

public:
    PluginManager(QObject *parent = 0);
    ~PluginManager();

    QStringList categories() const;
    QMap<QString, Plugin *> plugins(const QString &category) const;

    Q_INVOKABLE QObject *getByName(const QString &name) const;
    Q_INVOKABLE QAbstractItemModel *itemModel(const QString &category);
    QString getFilter();
    void setFilter(const QString &filter);

    // reimplemented virtual methods
    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void filterChanged();

private:
    PluginManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(PluginManager)
    QString m_filter;
};

} // namespace

#endif // SYSTEM_SETTINGS_PLUGIN_MANAGER_H
