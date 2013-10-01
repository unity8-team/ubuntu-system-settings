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

#include "utils.h"

#include <QDebug>
#include <QObject>
#include <QTest>

using namespace SystemSettings;

class ArgumentsTest: public QObject
{
    Q_OBJECT

public:
    ArgumentsTest() {};

private Q_SLOTS:
    void testNull();
    void testDefaultPlugin();
    void testPluginOptions();
    void testObsoleted();
};

void ArgumentsTest::testNull()
{
    QStringList args(QStringLiteral("appName"));
    QString defaultPlugin;
    QVariantMap pluginOptions;
    parsePluginOptions(args, defaultPlugin, pluginOptions);

    QCOMPARE(defaultPlugin, QString());
    QCOMPARE(pluginOptions, QVariantMap());
}

void ArgumentsTest::testDefaultPlugin()
{
    QStringList args(QStringLiteral("appName"));
    args << QStringLiteral("settings://system/myPlugin");

    QString defaultPlugin;
    QVariantMap pluginOptions;
    parsePluginOptions(args, defaultPlugin, pluginOptions);

    QCOMPARE(defaultPlugin, QStringLiteral("myPlugin"));
    QCOMPARE(pluginOptions, QVariantMap());

    /* Also check the host-less syntax */
    args = QStringList(QStringLiteral("appName"));
    args << QStringLiteral("settings:///system/myPlugin");

    defaultPlugin.clear();
    parsePluginOptions(args, defaultPlugin, pluginOptions);

    QCOMPARE(defaultPlugin, QStringLiteral("myPlugin"));
    QCOMPARE(pluginOptions, QVariantMap());
}

void ArgumentsTest::testPluginOptions()
{
    QStringList args(QStringLiteral("appName"));
    args << QStringLiteral("settings:///system/myPlugin?greeting=hello&count=1");

    QString defaultPlugin;
    QVariantMap pluginOptions;
    parsePluginOptions(args, defaultPlugin, pluginOptions);

    QCOMPARE(defaultPlugin, QStringLiteral("myPlugin"));
    QVariantMap expectedOptions;
    expectedOptions.insert("greeting", QStringLiteral("hello"));
    expectedOptions.insert("count", QStringLiteral("1"));
    QCOMPARE(pluginOptions, expectedOptions);
}

void ArgumentsTest::testObsoleted()
{
    QStringList args(QStringLiteral("appName"));
    args << QStringLiteral("myPlugin");
    args << QStringLiteral("--option") << QStringLiteral("greeting=hello");
    args << QStringLiteral("--option") << QStringLiteral("count=1");

    QString defaultPlugin;
    QVariantMap pluginOptions;
    parsePluginOptions(args, defaultPlugin, pluginOptions);

    QCOMPARE(defaultPlugin, QStringLiteral("myPlugin"));
    QVariantMap expectedOptions;
    expectedOptions.insert("greeting", QStringLiteral("hello"));
    expectedOptions.insert("count", QStringLiteral("1"));
    QCOMPARE(pluginOptions, expectedOptions);
}

QTEST_MAIN(ArgumentsTest);
#include "tst_arguments.moc"
