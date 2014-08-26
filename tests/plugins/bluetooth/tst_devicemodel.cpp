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
#include "devicemodel.h"
#include "fakebluez.h"

using namespace Bluez;

class DeviceModelTest: public QObject
{
    Q_OBJECT

private:
    FakeBluez *m_bluezMock;
    DeviceModel *m_devicemodel;
    QDBusConnection *m_dbus;

private Q_SLOTS:
    void init();
    void testDeviceFoundOnStart();
    void testDeviceFound();
    void testGetDeviceFromAddress();
    void testGetDeviceFromPath();
    void cleanup();

};

void DeviceModelTest::init()
{
    m_bluezMock = new FakeBluez();

    m_bluezMock->addAdapter("new0", "bluetoothTest");
    m_bluezMock->addDevice("My Phone", "00:00:de:ad:be:ef");

    m_dbus = new QDBusConnection(m_bluezMock->dbus());
    m_devicemodel = new DeviceModel(*m_dbus);
}

void DeviceModelTest::cleanup()
{
    delete m_bluezMock;
    delete m_devicemodel;
}

void DeviceModelTest::testDeviceFoundOnStart()
{
    QCOMPARE(m_devicemodel->rowCount(), 1);
}

void DeviceModelTest::testDeviceFound()
{
    QSKIP("TODO: Finish me.", SkipAll);

    m_bluezMock->addDevice("My New Phone", "00:0b:ad:c0:ff:ee");
    QCOMPARE(m_devicemodel->rowCount(), 2);
}

void DeviceModelTest::testGetDeviceFromAddress()
{
    auto device = m_devicemodel->getDeviceFromAddress("00:00:de:ad:be:ef");

    QVERIFY(device);
    QVERIFY(!device->getPath().isEmpty());
}

void DeviceModelTest::testGetDeviceFromPath()
{
    QList<QString> devices = m_bluezMock->devices();

    auto device = m_devicemodel->getDeviceFromPath(devices.at(0));

    QVERIFY(device);
    QVERIFY(!device->getPath().isEmpty());
}

QTEST_MAIN(DeviceModelTest)
#include "tst_devicemodel.moc"
