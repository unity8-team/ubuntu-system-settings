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

#include "plugin-manager.h"
#include "plugin.h"

#include <QDebug>
#include <QObject>
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
};

void PluginsTest::testCategory()
{
    PluginManager manager;

    QSet<QString> expectedCategories;
    expectedCategories << "phone" << "network";
    QCOMPARE(manager.categories().toSet(), expectedCategories);

    QList<Plugin *> plugins = manager.plugins("phone");
    QCOMPARE(plugins.count(), 1);
    QCOMPARE(plugins[0]->displayName(), QString("Cellular"));

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

QTEST_MAIN(PluginsTest);
#include "tst_plugins.moc"
