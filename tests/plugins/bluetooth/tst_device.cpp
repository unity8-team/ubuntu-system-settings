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
#include "fakebluez.h"

using namespace Bluez;

class DeviceTest: public QObject
{
    Q_OBJECT

private:
    FakeBluez *m_bluezMock;
    Device *m_device;
    QDBusConnection *m_dbus;

private:
    // void checkAudioState(const QString &expected);
    void processEvents(unsigned int msecs = 100);

private Q_SLOTS:
    void init();
    void testGetName();
    void testGetAddress();
    void testGetIconName();
    void testGetType();
    void testIsPaired();
    void testIsTrusted();
    void testGetConnection();
    void testGetStrength();
    void testGetPath();
    void testMakeTrusted();
    void testConnect();
    void testDisconnect();

    void cleanup();

};

void DeviceTest::processEvents(unsigned int msecs)
{
    QTimer::singleShot(msecs, [=]() { QCoreApplication::instance()->exit(); });
    QCoreApplication::instance()->exec();
}

void DeviceTest::init()
{
    qDBusRegisterMetaType<InterfaceList>();
    qDBusRegisterMetaType<ManagedObjectList>();

    m_bluezMock = new FakeBluez();
    m_bluezMock->addAdapter("new0", "bluetoothTest");
    m_bluezMock->addDevice("My Phone", "00:00:de:ad:be:ef");
    // Only this will set the 'Class' and 'Icon' properties for the device ...
    m_bluezMock->pairDevice("00:00:de:ad:be:ef");
    m_dbus = new QDBusConnection(m_bluezMock->dbus());

    QList<QString> devices = m_bluezMock->devices();
    if (devices.isEmpty())
        QFAIL("No devices in mock to be tested.");

    m_device = new Device(devices.first(), *m_dbus);

    processEvents();
}

void DeviceTest::cleanup()
{
    delete m_bluezMock;
    delete m_device;
}

void DeviceTest::testGetName()
{
    QCOMPARE(m_device->getName(), QString("My Phone"));
}

void DeviceTest::testGetAddress()
{
    QCOMPARE(m_device->getAddress(), QString("00:00:de:ad:be:ef"));
}

void DeviceTest::testGetIconName()
{
    QCOMPARE(m_device->getIconName(), QString("image://theme/phone-smartphone-symbolic"));
}

void DeviceTest::testGetType()
{
    QCOMPARE(m_device->getType(), Device::Type::Smartphone);
}

void DeviceTest::testIsPaired()
{
    QCOMPARE(m_device->isPaired(), true);

    m_bluezMock->setProperty("/org/bluez/new0/dev_00_00_DE_AD_BE_EF", "org.bluez.Device1", "Paired", QVariant(false));

    processEvents();

    QCOMPARE(m_device->isPaired(), false);

    m_bluezMock->setProperty("/org/bluez/new0/dev_00_00_DE_AD_BE_EF", "org.bluez.Device1", "Paired", QVariant(true));

    processEvents();

    QCOMPARE(m_device->isPaired(), true);
}

void DeviceTest::testIsTrusted()
{
    QCOMPARE(m_device->isTrusted(), false);

    m_bluezMock->setProperty("/org/bluez/new0/dev_00_00_DE_AD_BE_EF", "org.bluez.Device1", "Trusted", QVariant(true));

    processEvents();

    QCOMPARE(m_device->isTrusted(), true);
}

void DeviceTest::testGetConnection()
{
    QCOMPARE(m_device->getConnection(), Device::Connection::Disconnected);
}

void DeviceTest::testGetStrength()
{
    QCOMPARE(m_device->getStrength(), Device::Strength::Fair);
}

void DeviceTest::testGetPath()
{
    QCOMPARE(m_device->getPath(),
             m_bluezMock->currentAdapterPath() + "/"
             + "dev_00_00_DE_AD_BE_EF");
}

void DeviceTest::testMakeTrusted()
{
    QCOMPARE(m_device->isTrusted(), false);

    m_device->makeTrusted(true);

    processEvents();

    QCOMPARE(m_device->isTrusted(), true);

    m_device->makeTrusted(false);

    processEvents();

    QCOMPARE(m_device->isTrusted(), false);
}

void DeviceTest::testConnect()
{
    m_device->connect();

    m_bluezMock->connectDevice("00:00:de:ad:be:ef");

    processEvents();

    QCOMPARE(m_device->getConnection(), Device::Connected);
}

void DeviceTest::testDisconnect()
{
    testConnect();

    m_device->disconnect();

    m_bluezMock->disconnectDevice("00:00:de:ad:be:ef");

    processEvents();

    QCOMPARE(m_device->getConnection(), Device::Disconnected);
}

QTEST_MAIN(DeviceTest)
#include "tst_device.moc"
