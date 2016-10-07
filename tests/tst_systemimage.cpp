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

#include "systemimage.h"
#include "mocks/system-image-dbus/fakesystemimagedbus.h"

#include <libqtdbusmock/MethodCall.h>

#include <QTest>
#include <QSignalSpy>

class TstSystemImage: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init()
    {
        QVariantMap parameters;
        parameters["device"] = "test";
        parameters["auto_download"] = 0;
        parameters["channel"] = "testchannel";
        parameters["build_number"] = 10;
        parameters["target_build_number"] = 42;
        parameters["version_detail"] =
            "foo=bar,tag=OTA-100,ubuntu=101,device=102,custom=103";

        m_siMock = new FakeSystemImageDbus(parameters);
        m_dbus = new QDBusConnection(m_siMock->dbus());
        m_mock = new QDBusInterface(SI_SERVICE,
                                    SI_MAIN_OBJECT,
                                    "org.freedesktop.DBus.Mock",
                                    *m_dbus);
        m_methodSpy = new QSignalSpy(
            m_mock, SIGNAL(MethodCalled(const QString &, const QVariantList &))
        );
        m_systemImage = new QSystemImage(*m_dbus);

        /* The following connections help us test DBus signals that are not
        mockable. See https://github.com/martinpitt/python-dbusmock/issues/23
        */
        connect(
            m_siMock, SIGNAL(mockAvailableStatusChanged(
                bool, bool, QString, int, QString, QString
            )),
            m_systemImage, SLOT(availableStatusChanged(
                bool, bool, QString, int, QString, QString
            ))
        );
        connect(m_siMock, SIGNAL(mockSettingChanged(QString, QString)),
                m_systemImage, SLOT(settingsChanged(QString, QString)));
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_systemImage, SIGNAL(destroyed(QObject*)));
        m_systemImage->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
        delete m_methodSpy;
        delete m_siMock;
    }
    void testDetailedVersionDetails()
    {
        QVariantMap details;
        details["foo"] = "bar";
        details["tag"] = "OTA-100";
        details["ubuntu"] = "101";
        details["device"] = "102";
        details["custom"] = "103";
        QCOMPARE(m_systemImage->detailedVersionDetails(), details);
    }
    void testVersionTag()
    {
        QCOMPARE(m_systemImage->versionTag(), QString("OTA-100"));
    }
    void testAvailableStatus_data()
    {
        QTest::addColumn<bool>("isAvailable");
        QTest::addColumn<bool>("downloading");
        QTest::addColumn<QString>("availableVersion");
        QTest::addColumn<int>("updateSize");
        QTest::addColumn<QString>("lastUpdateDate");
        QTest::addColumn<QString>("errorReason");
        QTest::addColumn<int>("targetBuildNumber");

        QString updateDate("2015-01-01T20:00:00");

        QTest::newRow("available, downloading") << true << true << "50" << 1000 << updateDate << "" << 50;
        QTest::newRow("available, paused") << true << false << "50" << 1000 << updateDate << "paused" << 50;

        // Target build number will be the one from Information call (at creation).
        QTest::newRow("not available") << false << false << "" << 0 << updateDate << "" << 42;
    }
    void testAvailableStatus()
    {
        QFETCH(bool, isAvailable);
        QFETCH(bool, downloading);
        QFETCH(QString, availableVersion);
        QFETCH(int, updateSize);
        QFETCH(QString, lastUpdateDate);
        QFETCH(QString, errorReason);
        QFETCH(int, targetBuildNumber);

        Q_EMIT m_siMock->mockAvailableStatusChanged(
            isAvailable, downloading, availableVersion, updateSize,
            lastUpdateDate, errorReason
        );

        QCOMPARE(m_systemImage->updateAvailable(), isAvailable);
        QCOMPARE(m_systemImage->downloading(), downloading);
        QCOMPARE(m_systemImage->targetBuildNumber(), targetBuildNumber);
        QCOMPARE(m_systemImage->updateSize(), updateSize);
        QCOMPARE(m_systemImage->errorReason(), errorReason);
    }
    void testDeviceName()
    {
        QCOMPARE(m_systemImage->deviceName(), QString("test"));
    }
    void testDownloadMode()
    {
        QCOMPARE(m_systemImage->downloadMode(), 0);
    }
    void testSetDownloadMode_data()
    {
        QTest::addColumn<int>("downloadMode");
        QTest::addColumn<int>("targetDownloadMode");
        QTest::newRow("never") << 0 << 0;
        QTest::newRow("wifi") << 1 << 1;
        QTest::newRow("always") << 2 << 2;

        // We expect these not not change the value (default is 0 in this test)
        QTest::newRow("too high") << 3 << 0;
        QTest::newRow("too low") << -1 << 0;
    }
    void testSetDownloadMode()
    {
        QFETCH(int, downloadMode);
        QFETCH(int, targetDownloadMode);

        m_systemImage->setDownloadMode(downloadMode);
        QCOMPARE(m_systemImage->downloadMode(), targetDownloadMode);
    }
    void testSettingsChangedRemotely()
    {
        Q_EMIT m_siMock->mockSettingChanged("auto_download", "2");
        QCOMPARE(m_systemImage->downloadMode(), 2);

        Q_EMIT m_siMock->mockSettingChanged("failures_before_warning", "80");
        QCOMPARE(m_systemImage->failuresBeforeWarning(), 80);
    }
    void testChannelName()
    {
        QCOMPARE(m_systemImage->channelName(), QString("testchannel"));
    }
    void testBuildNumbers()
    {
        QCOMPARE(m_systemImage->currentUbuntuBuildNumber(), QString("101"));
        QCOMPARE(m_systemImage->currentDeviceBuildNumber(), QString("102"));
        QCOMPARE(m_systemImage->currentCustomBuildNumber(), QString("103"));
    }
    void testCheckForUpdate()
    {
        m_systemImage->checkForUpdate();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "CheckForUpdate");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.
    }
    void testDownloadUpdate()
    {
        m_systemImage->downloadUpdate();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "DownloadUpdate");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.
    }
    void testForceAllowGSMDownload()
    {
        m_systemImage->forceAllowGSMDownload();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "ForceAllowGSMDownload");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.
    }
    void testApplyUpdate()
    {
        m_systemImage->applyUpdate();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "ApplyUpdate");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.
    }
    void testCancelUpdate()
    {
        QString reason = m_systemImage->cancelUpdate();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "CancelUpdate");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.

        // cancelUpdate should also return an empty string (i.e. no error)
        QCOMPARE(reason, QString(""));
    }
    void testPauseDownload()
    {
        QString reason = m_systemImage->pauseDownload();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "PauseDownload");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");

        // pauseDownload should also return an empty string (i.e. no error)
        QCOMPARE(reason, QString(""));
    }
    void testProductionReset()
    {
        m_systemImage->productionReset();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "ProductionReset");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.
    }
    void testFactoryReset()
    {
        m_systemImage->factoryReset();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "FactoryReset");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.
    }
    void testCheckTarget()
    {
        // Should return true, see init().
        QVERIFY(m_systemImage->checkTarget());
    }
    void testCheckingForUpdates()
    {
        QSignalSpy checkingChangedSpy(
            m_systemImage, SIGNAL(checkingForUpdatesChanged())
        );

        // Check starts.
        m_systemImage->checkForUpdate();
        QTRY_COMPARE(checkingChangedSpy.count(), 1);
        QVERIFY(m_systemImage->checkingForUpdates());

        // Check ends.
        Q_EMIT m_siMock->mockAvailableStatusChanged(
            false, false, "", 0, "", "" // doesn't matter
        );
        QTRY_COMPARE(checkingChangedSpy.count(), 2);
        QVERIFY(!m_systemImage->checkingForUpdates());
    }
    void testFailuresBeforeWarning()
    {
        // 3 is the default.
        QCOMPARE(m_systemImage->failuresBeforeWarning(), 3);
    }
private:
    QSignalSpy *m_methodSpy;
    FakeSystemImageDbus *m_siMock = nullptr;
    QDBusInterface *m_mock = nullptr;
    QSystemImage *m_systemImage = nullptr;
    QDBusConnection *m_dbus = nullptr;
};

QTEST_GUILESS_MAIN(TstSystemImage)
#include "tst_systemimage.moc"
