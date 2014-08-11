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
    void checkAudioState(const QString &expected);

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
    void testDiscoverServices();
    
    void cleanup();

};

void DeviceTest::init()
{
    m_bluezMock = new FakeBluez();
    m_bluezMock->addAdapter("new0", "bluetoothTest");
    m_bluezMock->addDevice("My Phone", "00:00:de:ad:be:ef");
    m_dbus = new QDBusConnection(m_bluezMock->dbus());

    QList<QString> devices = m_bluezMock->devices();
    if (devices.isEmpty())
        QFAIL("No devices in mock to be tested.");

    m_device = new Device(devices.first(), *m_dbus);
}

void DeviceTest::cleanup()
{
    delete m_bluezMock;
    delete m_device;
}

void DeviceTest::testGetName()
{
    QCOMPARE(m_device->getName(), "My Phone");
}

void DeviceTest::testAddress()
{
    QCOMPARE(m_device->getAddress(), "00:00:de:ad:be:ef");
}

void DeviceTest::testIconName()
{
    QCOMPARE(m_device->getIconName(), "image://theme/audio-headset");
}

void DeviceTest::testGetType()
{
    QCOMPARE(m_device->getType(), Device::Type::Headset);
}

void DeviceTest::testIsPaired()
{
    QCOMPARE(m_device->isPaired(), false);

    m_bluezMock->setProperty("org.bluez.Device", "Paired", QVariant(true));

    QCOMPARE(m_device->isPaired(), true);
}

void DeviceTest::testIsTrusted()
{
    QCOMPARE(m_device->isTrusted(), false);

    m_bluezMock->setProperty("org.bluez.Device", "Trusted", QVariant(true));

    QCOMPARE(m_device->isTrusted(), true);
}

void DeviceTest::testGetConnection()
{
    QCOMPARE(m_device.getConnection(), Device::Connection::Disconnected);
}

void DeviceTest::testGetStrength()
{
    QCOMPARE(m_device.getStrength(), Device::Strength::Good);
}

void DeviceTest::testGetPath()
{
    QCOMPARE(m_device.getPath(),
             "/org/bluez/" + m_bluezMock->currentAdapter() + "/"
             + "dev_00_00_de_ad_be_ef");
}

void DeviceTest::testMakeTrusted()
{
    QCOMPARE(m_device->isTrusted(), false);

    m_device->makeTrusted(true);

    QCOMPARE(m_device->isTrusted(), true);

    m_device->makeTrusted(false);

    QCOMPARE(m_device->isTrusted(), false);
}

void DeviceTest::checkAudioState(const QString &expected)
{
    QVariant state = m_bluezMock->getProperty("org.bluez.Audio", "State");

    QVERIFY(state.type() == QMetaType::QString);
    QCOMPARE(state.toString(), expected);
}

void DeviceTest::testDiscoverServices()
{
    m_device->discoverServices();

    QVariant state = m_bluezMock->getProperty("org.bluez.Audio", "State");

    checkAudioState("disconnected");
}

void DeviceTest::testConnect()
{
    testDiscoverServices();

    m_device->connect(Device::ConnectionMode::Audio);

    checkAudioState("connected");
}

void DeviceTest::testDisconnect()
{
    testConnect();

    m_device->disconnect(Device::ConnectionMode::Audio);

    checkAudioState("disconnected");
}

void DeviceTest::testConnectPending()
{
    testIsPaired();
    testIsTrusted();

    m_device->addConnectAfterPairing(Device::ConnectionMode::Audio);

    checkAudioState("disconnected");

    m_device->connectPending();

    checkAudioState("connected");
}

QTEST_MAIN(DeviceTest);
#include "tst_device.moc"
