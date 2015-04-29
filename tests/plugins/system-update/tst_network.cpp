/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
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

#include <QDebug>
#include <QtQml>
#include <QtQml/QQmlContext>
#include <QObject>
#include <QTest>
#include <QString>
#include "network/network.h"
#include "update.h"

using namespace UpdatePlugin;

class NetworkTest: public QObject
{
    Q_OBJECT

public:
    NetworkTest() {};

private Q_SLOTS:
    void testArch();
    void testFrameworks();

};

void NetworkTest::testFrameworks()
{
    Network net;
    auto frameworks = net.get_available_frameworks();
    QCOMPARE(frameworks.empty(), false);
}

void NetworkTest::testArch()
{
    Network net;
    auto arch = net.get_architecture();
    QCOMPARE(arch.empty(), false);
}

QTEST_MAIN(NetworkTest)
#include "tst_network.moc"
