/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#include "wifidbushelper.h"
#include "fakenetworkmanager.h"

#include <libqtdbusmock/MethodCall.h>

#include <QDBusMetaType>
#include <QTest>
#include <QSignalSpy>

class TstDbusHelper: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init()
    {
        QVariantMap parameters;
        m_nmMock = new FakeNetworkManager(parameters);
        m_dbus = new QDBusConnection(m_nmMock->dbus());
        m_instance = new WifiDbusHelper(*m_dbus);
        m_mock = new QDBusInterface(NM_SERVICE,
                                    NM_MAIN_OBJECT,
                                    "org.freedesktop.DBus.Mock",
                                    *m_dbus);
        m_methodSpy = new QSignalSpy(
            m_mock, SIGNAL(MethodCalled(const QString&, const QVariantList&))
        );

        // Add a Wi-Fi device.
        auto devCall = m_mock->asyncCall("AddWiFiDevice", "0", "wlan0", 100);
        QDBusPendingCallWatcher devWatcher(devCall);
        devWatcher.waitForFinished();
        auto reply = QDBusPendingReply<QString>(devWatcher);
        if (reply.isError()) {
            QFAIL("Failed to create device");
        } else {
            m_devPath = reply.argumentAt<0>();
        }

        // Add access points.
        auto wpa = QList<QVariant>();
        wpa << m_devPath << "test_ap_wpa" << "test_ap_wpa" << "00:00:00:00:00:00" << (uint) 3
            << (uint) 60 << (uint) 128 << QVariant::fromValue(uchar(0)) << (uint) 0x00000100;
        m_mock->callWithArgumentList(QDBus::Block, "AddAccessPoint", wpa);

        auto wpaep = QList<QVariant>();
        wpaep << m_devPath << "test_ap_wpaep" << "test_ap_wpaep" << "11:11:11:11:11:11" << (uint) 3
              << (uint) 60 << (uint) 128 << QVariant::fromValue(uchar(0)) << (uint) 0x00000200;
        m_mock->callWithArgumentList(QDBus::Block, "AddAccessPoint", wpaep);
    }
    void cleanup()
    {
        m_devPath = QString::null;
        delete m_methodSpy;
        delete m_nmMock;
        delete m_instance;
        delete m_mock;
        delete m_dbus;
    }
    void testConnect_data()
    {
        QTest::addColumn<QString>("ssid");
        QTest::addColumn<int>("security");
        QTest::addColumn<int>("auth");
        QTest::addColumn<QStringList>("usernames");
        QTest::addColumn<QStringList>("password");
        QTest::addColumn<QStringList>("certs");
        QTest::addColumn<int>("p2auth");

        {
            QString ssid("test_ap_wpa");
            int security = 1;
            int auth = 0;
            QStringList usernames;
            usernames << "user" << "" << "";
            QStringList password;
            password << "password" << "false";
            QStringList certs;
            certs << "" << "" << "" << "" << "" << "";
            int p2auth = 0;

            QTest::newRow("WPA2") << ssid << security << auth << usernames << password << certs << p2auth;
        }
        {
            QString ssid("test_ap_wpaep");
            int security = 2;
            int auth = 0;
            QStringList usernames;
            usernames << "user" << "" << "";
            QStringList password;
            password << "password" << "false";
            QStringList certs;
            certs << "" << "" << "" << "" << "" << "";
            int p2auth = 0;

            QTest::newRow("WPA2 Enterprise") << ssid << security << auth << usernames << password << certs << p2auth;
        }
    }
    void testConnect()
    {
        QFETCH(QString, ssid);
        QFETCH(int, security);
        QFETCH(int, auth);
        QFETCH(QStringList, usernames);
        QFETCH(QStringList, password);
        QFETCH(QStringList, certs);
        QFETCH(int, p2auth);

        QSignalSpy activeConnectionSpy(
            m_mock, SIGNAL(MethodCalled(const QString &, const QVariantList &))
        );

        m_instance->connect(ssid, security, auth, usernames, password, certs, p2auth);

        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "AddAndActivateConnection");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.

        // Ensure active connection is that of the ssid we connected to.
        auto path_v = m_nmMock->getProperty(m_devPath,
                                            "org.freedesktop.NetworkManager.Device",
                                            "ActiveConnection");
        auto path = qdbus_cast<QDBusObjectPath>(path_v).path();
        QCOMPARE(path,
                 QString("/org/freedesktop/NetworkManager/ActiveConnection/%1")
                    .arg(ssid));

        // Ensure device state is OK (100).
        auto state_v = m_nmMock->getProperty(m_devPath,
                                             "org.freedesktop.NetworkManager.Device",
                                             "State");
        auto state = qdbus_cast<uint>(state_v);
        QCOMPARE(state, (uint) 100);
    }
private:
    QSignalSpy *m_methodSpy;
    FakeNetworkManager *m_nmMock;
    QDBusInterface *m_mock;
    WifiDbusHelper *m_instance;
    QDBusConnection *m_dbus;
    QString m_devPath = QString::null;
};

QTEST_MAIN(TstDbusHelper)
#include "tst_wifidbushelper.moc"
