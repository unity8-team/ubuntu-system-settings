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

#include "test-plugin.h"

#include <QStringList>
#include <SystemSettings/PluginBase>

using namespace SystemSettings;

class TestItem: public PluginBase
{
    Q_OBJECT

public:
    TestItem(const QVariantMap &staticData, QObject *parent = 0);
    ~TestItem();

    virtual QQmlComponent *entryComponent(QQmlEngine *engine,
                                          QObject *parent = 0);
    virtual QQmlComponent *pageComponent(QQmlEngine *engine,
                                         QObject *parent = 0);
};

TestItem::TestItem(const QVariantMap &staticData, QObject *parent):
    PluginBase(staticData, parent)
{
    QStringList keywords;
    keywords << "one" << "two" << "three";
    setKeywords(keywords);
}

TestItem::~TestItem()
{
}

QQmlComponent *TestItem::entryComponent(QQmlEngine *engine, QObject *parent)
{
    Q_UNUSED(engine);
    Q_UNUSED(parent);
    return 0;
}

QQmlComponent *TestItem::pageComponent(QQmlEngine *engine, QObject *parent)
{
    QQmlComponent *page = new QQmlComponent(engine, parent);
    page->setData("import QtQuick 2.0\n"
                  "Rect {\n"
                  "  width: 200; height: 200;\n"
                  "  objectName: \"myRect\"\n"
                  "}",
                  QUrl());
    return page;
}

TestPlugin::TestPlugin():
    QObject()
{
}

PluginBase *TestPlugin::createPlugin(const QVariantMap &staticData,
                                     QObject *parent)
{
    return new TestItem(staticData, parent);
}

#include "test-plugin.moc"
