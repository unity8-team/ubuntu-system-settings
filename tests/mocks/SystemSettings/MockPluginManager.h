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
#include <QQmlComponent>

class MockItem;
class MockItemModel;
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
    void addPlugin(const QString &name,
                   QQmlComponent *entry,
                   QQmlComponent *page,
                   const QString &category = "uncategorized-bottom");

Q_SIGNALS:
    void filterChanged();

private:
    QString m_filter = QString::null;
    QMap<QString, MockItemModel*> m_models;
    QMap<QString, MockItem*> m_plugins;
};

class MockItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *entryComponent READ entryComponent CONSTANT)
    Q_PROPERTY(QQmlComponent *pageComponent READ pageComponent CONSTANT)
    Q_PROPERTY(bool visible READ visible CONSTANT)
    Q_PROPERTY(QString baseName READ baseName CONSTANT)
public:
    explicit MockItem(QObject *parent = 0) : QObject(parent) {};
    ~MockItem() {};
    QQmlComponent* entryComponent();
    void setEntryComponent(QQmlComponent* c);
    QQmlComponent* pageComponent();
    void setPageComponent(QQmlComponent* c);
    bool visible() const;
    QString baseName() const;
    void setBaseName(const QString &baseName);
private:
    QQmlComponent* m_entry;
    QQmlComponent* m_page;
    QString m_baseName;
};

class MockItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MockItemModel(QObject *parent = 0);
    ~MockItemModel() {};

    enum Roles {
        IconRole = Qt::UserRole + 1,
        ItemRole,
    };
    void addPlugin(MockItem *plugin);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

private:
    QList<MockItem*> m_plugins;
};

#endif // MOCK_PLUGIN_MANAGER_H
