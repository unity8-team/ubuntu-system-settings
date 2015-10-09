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
#include "bluez_helper.h"
#include "fakebluez.h"

using namespace Bluez;

class BluetoothTest: public QObject
{
    Q_OBJECT

private:
    FakeBluez *m_bluezMock;
    Bluetooth *m_bluetooth;
    QDBusConnection *m_dbus;

    void processEvents(unsigned int msecs = 100);
    void setDiscovering(bool value);

private Q_SLOTS:
    void init();
    void testGotAdapter();
    void testStartDiscovery();
    void testStopDiscovery();
    void testToggleDiscovery();
    void testIsSupportedType();
    void testIsDiscovering();
    void cleanup();

};

void BluetoothTest::processEvents(unsigned int msecs)
{
    QTimer::singleShot(msecs, [=]() { QCoreApplication::instance()->exit(); });
    QCoreApplication::instance()->exec();
}

void BluetoothTest::setDiscovering(bool value)
{
    m_bluezMock->setProperty(m_bluezMock->currentAdapterPath(),
                             BLUEZ_ADAPTER_IFACE,
                             "Discovering",
                             QVariant(value));
}

void BluetoothTest::init()
{
    qWarning() << "init test";

    qDBusRegisterMetaType<InterfaceList>();
    qDBusRegisterMetaType<ManagedObjectList>();

    m_bluezMock = new FakeBluez();
    m_bluezMock->addAdapter("new0", "bluetoothTest");
    m_dbus = new QDBusConnection(m_bluezMock->dbus());
    m_bluetooth = new Bluetooth(*m_dbus);

    processEvents();
}

void BluetoothTest::cleanup()
{
    qWarning() << "cleanup";
    delete m_bluezMock;
    delete m_bluetooth;
}

void BluetoothTest::testGotAdapter()
{
    QString expected = "bluetoothTest";
    QString result;

    processEvents();

    result = m_bluetooth->adapterName();

    QCOMPARE(result, expected);
}

void BluetoothTest::testStartDiscovery()
{
    QVariant result;

    // This is what our test expects the adapter to have set
    setDiscovering(false);
    processEvents();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapterPath(),
                                      BLUEZ_ADAPTER_IFACE,
                                      "Discovering");
    qWarning() << result;
    QCOMPARE(result.toBool(), false);

    m_bluetooth->startDiscovery();
    setDiscovering(true);

    processEvents();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapterPath(),
                                      BLUEZ_ADAPTER_IFACE,
                                      "Discovering");
    qWarning() << result;
    QCOMPARE(result.toBool(), true);
}

void BluetoothTest::testStopDiscovery()
{
    QVariant result;

    // This is what our test expects the adapter to have set
    setDiscovering(true);
    processEvents();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapterPath(),
                                      BLUEZ_ADAPTER_IFACE,
                                      "Discovering");
    QCOMPARE(result.toBool(), true);

    m_bluetooth->stopDiscovery();
    setDiscovering(false);

    processEvents();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapterPath(),
                                      BLUEZ_ADAPTER_IFACE,
                                      "Discovering");
    QCOMPARE(result.toBool(), false);
}

/*
 * NOTE: The bluez5 mock template currently doesn't send PropertiesChanged
 * events when StartDiscovery/StopDiscovery is called on the adapter interface.
 * To accomondate this we're calling the org.freedesktop.DBus.Properties.Set
 * method here manually to simulate a property change. However this means
 * that other than doing a dumb call to StartDiscovery/StopDiscovery nothing
 * else will happen when those methods are called of the Bluetooth class we're
 * testing here.
 *
 * This affects the following tested methods:
 * - Bluetooth::startDiscovering
 * - Bluetooth::stopDiscovery
 * - Bluetooth::toggleDiscovery
 */

void BluetoothTest::testToggleDiscovery()
{
    QVariant result;

    m_bluetooth->stopDiscovery();
    setDiscovering(false);

    processEvents();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapterPath(),
                                      BLUEZ_ADAPTER_IFACE,
                                      "Discovering");
    QCOMPARE(result.toBool(), false);

    m_bluetooth->toggleDiscovery();
    setDiscovering(true);

    processEvents();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapterPath(),
                                      BLUEZ_ADAPTER_IFACE,
                                      "Discovering");
    QCOMPARE(result.toBool(), true);

    m_bluetooth->toggleDiscovery();
    setDiscovering(false);

    processEvents();

    result = m_bluezMock->getProperty(m_bluezMock->currentAdapterPath(),
                                      BLUEZ_ADAPTER_IFACE,
                                      "Discovering");
    QCOMPARE(result.toBool(), false);
}

void BluetoothTest::testIsSupportedType()
{
    QCOMPARE(Bluetooth::isSupportedType(Device::Type::Headset), true);
    QCOMPARE(Bluetooth::isSupportedType(Device::Type::Keyboard), true);
    QCOMPARE(Bluetooth::isSupportedType(Device::Type::Tablet), false);
}


void BluetoothTest::testIsDiscovering()
{
    m_bluetooth->stopDiscovery();
    setDiscovering(false);

    processEvents();

    QCOMPARE(m_bluetooth->isDiscovering(), false);

    m_bluetooth->startDiscovery();
    setDiscovering(true);

    processEvents();

    QCOMPARE(m_bluetooth->isDiscovering(), true);

    m_bluetooth->toggleDiscovery();
    setDiscovering(false);

    processEvents();

    QCOMPARE(m_bluetooth->isDiscovering(), false);

    m_bluetooth->toggleDiscovery();
    setDiscovering(true);

    processEvents();

    QCOMPARE(m_bluetooth->isDiscovering(), true);
}

QTEST_MAIN(BluetoothTest)
#include "tst_bluetooth.moc"
