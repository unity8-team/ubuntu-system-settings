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
    void testName();
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

    QMap<QString, Plugin *> plugins = manager.plugins();
    QCOMPARE(plugins.count(), 5);

    QStringList categories;
    Q_FOREACH(Plugin *plugin, plugins) {
        categories << plugin->category();
    }
    QSet<QString> expectedCategories;
    expectedCategories << "phone" << "network" << "misc" << "system";
    QCOMPARE(categories.toSet(), expectedCategories);
}

void PluginsTest::testName()
{
    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    Plugin *brightness = 0;
    Q_FOREACH(Plugin *plugin, manager.plugins()) {
        if (plugin->displayName() == "Brightness & Display") {
            brightness = plugin;
        }
    }

    QVERIFY(brightness != 0);

    QCOMPARE(brightness->displayName(), QString("Brightness & Display"));
}

void PluginsTest::testKeywords()
{
    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    Plugin *wireless = 0;
    Plugin *bluetooth = 0;
    Q_FOREACH(Plugin *plugin, manager.plugins()) {
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
    /*
    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    QAbstractItemModel *model(manager.itemModel());

    QVERIFY(model != 0);
    QCOMPARE(model->rowCount(), 5);

    Plugin *wireless = (Plugin *) model->data(model->index(0, 0),
                                         ItemModel::ItemRole).value<QObject *>();
    Plugin *cellular = (Plugin *) model->data(model->index(1, 0),
                                             ItemModel::ItemRole).value<QObject *>();

    QCOMPARE(wireless->displayName(), QString("Wireless"));
    QCOMPARE(cellular->displayName(), QString("Bluetooth"));
    */
}

void PluginsTest::testReset()
{

    PluginManager manager;
    manager.classBegin();
    manager.componentComplete();

    Plugin *wireless = (Plugin *) manager.getByName("wireless");
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

    Plugin *phone = (Plugin *) manager.getByName("phone");
    QQmlEngine engine;
    QQmlContext *context = new QQmlContext(engine.rootContext());
    QQmlEngine::setContextForObject(phone, context);

    /* This is how you check that a debug message was printed */
    /* qDebug() inserts a space at the end */
    QTest::ignoreMessage(QtDebugMsg, "reset function in plugin ");

    phone->reset();
}

QTEST_MAIN(PluginsTest)
#include "tst_plugins.moc"
