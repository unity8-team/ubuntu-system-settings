/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QTest>
#include <QSignalSpy>
#include <QThread>

#include "bluetooth.h"
#include "device.h"
#include "agent.h"
#include "fakebluez.h"

using namespace Bluez;

class BluetoothTest: public QObject
{
    Q_OBJECT

private:
    FakeBluez *m_bluezMock;
    Bluetooth *m_bluetooth;
    QDBusConnection *m_dbus;

private Q_SLOTS:
    void init();
    void testGotAdapter();
    void testStartDiscovery();
    void testStopDiscovery();
    void testToggleDiscovery();
    void testIsDiscovering();
    void cleanup();

};

void BluetoothTest::init()
{
    m_bluezMock = new FakeBluez();
    m_bluezMock->addAdapter("new0", "bluetoothTest");
    m_dbus = new QDBusConnection(m_bluezMock->dbus());
    m_bluetooth = new Bluetooth(*m_dbus);
}

void BluetoothTest::cleanup()
{
    delete m_bluezMock;
    delete m_bluetooth;
}

void BluetoothTest::testGotAdapter()
{
    QString expected = "bluetoothTest";
    QString result;

    result = m_bluetooth->adapterName();

    QCOMPARE(result, expected);
}

void BluetoothTest::testStartDiscovery()
{
    bool expected = true;
    QVariant result;

    QSKIP("Fails due to a bug in bluez4 dbusmock template", SkipAll);

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapter(),
                                      "org.bluez.Adapter",
                                      "Discovering");
    qWarning() << result;
    QCOMPARE(result.toBool(), !expected);

    m_bluetooth->startDiscovery();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapter(),
                                      "org.bluez.Adapter",
                                      "Discovering");
    qWarning() << result;
    QCOMPARE(result.toBool(), expected);
}

void BluetoothTest::testStopDiscovery()
{
    bool expected = false;
    QVariant result;

    QSKIP("Fails due to a bug in bluez4 dbusmock template", SkipAll);

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapter(),
                                      "org.bluez.Adapter",
                                      "Discovering");
    QCOMPARE(result.toBool(), !expected);

    m_bluetooth->stopDiscovery();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapter(),
                                      "org.bluez.Adapter",
                                      "Discovering");
    QCOMPARE(result.toBool(), expected);
}

void BluetoothTest::testToggleDiscovery()
{
    QVariant result;

    QSKIP("Fails due to a bug in bluez4 dbusmock template", SkipAll);

    m_bluetooth->stopDiscovery();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapter(),
                                      "org.bluez.Adapter",
                                      "Discovering");
    QCOMPARE(result.toBool(), false);

    m_bluetooth->toggleDiscovery();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapter(),
                                      "org.bluez.Adapter",
                                      "Discovering");
    QCOMPARE(result.toBool(), true);

    m_bluetooth->toggleDiscovery();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapter(),
                                      "org.bluez.Adapter",
                                      "Discovering");
    QCOMPARE(result.toBool(), false);
}

void BluetoothTest::testIsDiscovering()
{
    m_bluetooth->stopDiscovery();

    QCOMPARE(m_bluetooth->isDiscovering(), false);

    m_bluetooth->startDiscovery();

    QCOMPARE(m_bluetooth->isDiscovering(), true);

    m_bluetooth->toggleDiscovery();

    QCOMPARE(m_bluetooth->isDiscovering(), false);

    m_bluetooth->toggleDiscovery();

    QCOMPARE(m_bluetooth->isDiscovering(), true);
}

QTEST_MAIN(BluetoothTest)
#include "tst_bluetooth.moc"
