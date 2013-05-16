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

#include "example-plugin.h"

#include <QDebug>
#include <QStringList>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

class ExampleItem: public ItemBase
{
    Q_OBJECT

public:
    ExampleItem(const QVariantMap &staticData, QObject *parent = 0);
    ~ExampleItem();

    virtual QQmlComponent *pageComponent(QQmlEngine *engine,
                                         QObject *parent = 0);
};

ExampleItem::ExampleItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
}

ExampleItem::~ExampleItem()
{
}

QQmlComponent *ExampleItem::pageComponent(QQmlEngine *engine, QObject *parent)
{
    return new QQmlComponent(engine,
                             QUrl("qrc:/PageComponent.qml"),
                             parent);
}

ExamplePlugin::ExamplePlugin():
    QObject()
{
}

ItemBase *ExamplePlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new ExampleItem(staticData, parent);
}

#include "example-plugin.moc"
