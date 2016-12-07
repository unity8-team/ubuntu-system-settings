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

#ifndef MOCK_PLUGIN_MANAGER_H
#define MOCK_PLUGIN_MANAGER_H

#include <QAbstractItemModel>
#include <QObject>
#include <QString>

class MockPluginManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString filter READ getFilter WRITE setFilter
                NOTIFY filterChanged)

public:
    explicit MockPluginManager(QObject *parent = nullptr);
    ~MockPluginManager() {};

public Q_SLOTS:
    QObject* getByName(const QString &name) const;
    QAbstractItemModel* itemModel(const QString &category);
    void resetPlugins();
    QString getFilter();
    void setFilter(const QString &filter);

Q_SIGNALS:
    void filterChanged();

private:
    QString m_filter = QString::null;
};

#endif // MOCK_PLUGIN_MANAGER_H
