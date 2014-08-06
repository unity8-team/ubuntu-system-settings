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

#include "item-model.h"
#include "plugin-manager.h"
#include "plugin.h"

#include <QDebug>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSignalSpy>
#include <QTest>

using namespace SystemSettings;

class PluginsTest: public QObject
{
    Q_OBJECT

public:
    PluginsTest() {};

private Q_SLOTS:
    void testCategory();
    void testKeywords();
    void testSorting();
    void testReset();
    void testResetInPlugin();
};

void PluginsTest::testCategory()
{
    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    QSet<QString> expectedCategories;
    expectedCategories << "phone" << "network" << "misc";
    QCOMPARE(manager.categories().toSet(), expectedCategories);

    QMap<QString, Plugin *> plugins = manager.plugins("phone");
    QCOMPARE(plugins.count(), 1);
    QCOMPARE(plugins.value("cellular")->displayName(), QString("Cellular"));

    plugins = manager.plugins("network");
    QCOMPARE(plugins.count(), 2);
    QStringList names;
    Q_FOREACH(Plugin *plugin, plugins) {
        names << plugin->displayName();
    }
    QSet<QString> expectedNames;
    expectedNames << "Bluetooth" << "Wireless";
    QCOMPARE(names.toSet(), expectedNames);
}

void PluginsTest::testKeywords()
{
    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    Plugin *wireless = 0;
    Plugin *bluetooth = 0;
    Q_FOREACH(Plugin *plugin, manager.plugins("network")) {
        if (plugin->displayName() == "Bluetooth") {
            bluetooth = plugin;
        } else if (plugin->displayName() == "Wireless") {
            wireless = plugin;
        }
    }
    QVERIFY(bluetooth != 0);
    QVERIFY(wireless != 0);

    QStringList keywords = bluetooth->keywords();
    QStringList expectedKeywords;
    expectedKeywords << "bluetooth";
    QCOMPARE(keywords, expectedKeywords);

    // The manifest has has-dynamic-keywords set, so the plugin will be loaded
    keywords = wireless->keywords();
    expectedKeywords.clear();
    expectedKeywords << "wireless" << "wlan" << "wifi" <<
        "one" << "two" << "three";
    QCOMPARE(keywords, expectedKeywords);
}

void PluginsTest::testSorting()
{
    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    QAbstractItemModel *model(manager.itemModel("network"));

    QVERIFY(model != 0);
    QCOMPARE(model->rowCount(), 2);

    Plugin *wireless = (Plugin *) model->data(model->index(0, 0),
                                         ItemModel::ItemRole).value<QObject *>();
    Plugin *cellular = (Plugin *) model->data(model->index(1, 0),
                                             ItemModel::ItemRole).value<QObject *>();

    QCOMPARE(wireless->displayName(), QString("Wireless"));
    QCOMPARE(cellular->displayName(), QString("Bluetooth"));
}

void PluginsTest::testReset()
{

    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    QAbstractItemModel *model(manager.itemModel("network"));
    Plugin *wireless = (Plugin *) model->data(model->index(0, 0),
                                         ItemModel::ItemRole).value<QObject *>();

    QQmlEngine engine;
    QQmlContext *context = new QQmlContext(engine.rootContext());
    QQmlEngine::setContextForObject(wireless, context);

    /* This is how you check that a debug message was printed */
    QTest::ignoreMessage(QtDebugMsg, "Hello");
    wireless->reset();
}

void PluginsTest::testResetInPlugin()
{
    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    QAbstractItemModel *model(manager.itemModel("misc"));
    Plugin *phone = (Plugin *) model->data(model->index(0, 0),
                                         ItemModel::ItemRole).value<QObject *>();

    QQmlEngine engine;
    QQmlContext *context = new QQmlContext(engine.rootContext());
    QQmlEngine::setContextForObject(phone, context);

    /* This is how you check that a debug message was printed */
    /* qDebug() inserts a space at the end */
    QTest::ignoreMessage(QtDebugMsg, "reset function in plugin ");

    phone->reset();
}

QTEST_MAIN(PluginsTest);
#include "tst_plugins.moc"
