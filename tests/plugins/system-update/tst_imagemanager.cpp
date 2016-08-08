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

#include "updatedb.h"
#include "systemimage.h"
#include "image/imagemanager_impl.h"

#include "system-image-dbus/fakesystemimagedbus.h"

#include <libqtdbusmock/MethodCall.h>

#include <QSignalSpy>
#include <QTest>
#include <QVariantMap>

using namespace UpdatePlugin;

Q_DECLARE_METATYPE(Update::State)

class TstImageManager : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_model = new UpdateModel(":memory:");

        QVariantMap parameters;
        parameters["build_number"] = 1;
        parameters["target_build_number"] = 2;
        m_siMock = new FakeSystemImageDbus(parameters);
        m_dbus = new QDBusConnection(m_siMock->dbus());
        m_systemImage = new QSystemImage(*m_dbus);
        m_mock = new QDBusInterface(SI_SERVICE,
                                    SI_MAIN_OBJECT,
                                    "org.freedesktop.DBus.Mock",
                                    *m_dbus);

        m_instance = new Image::ManagerImpl(m_systemImage, m_model);

        m_methodSpy = new QSignalSpy(
            m_mock, SIGNAL(MethodCalled(const QString &, const QVariantList &))
        );

        connect(this, SIGNAL(mockCheckingForUpdatesChanged()),
                m_instance, SLOT(handleCheckingForUpdatesChanged()));
        connect(this,
            SIGNAL(mockUpdateAvailableStatus(const bool, const bool, const QString&,
                                             const int&, const QString&,
                                             const QString&)),
            m_instance,
            SLOT(handleUpdateAvailableStatus(const bool, const bool, const QString&,
                                             const int&, const QString&,
                                             const QString&))
        );
        connect(this, SIGNAL(mockDownloadStarted()),
                m_instance, SLOT(handleDownloadStarted()));
        connect(this, SIGNAL(mockUpdateProgress(const int&, const double&)),
                m_instance, SLOT(handleUpdateProgress(const int&, const double&)));
        connect(this, SIGNAL(mockUpdatePaused(const int&)),
                m_instance, SLOT(handleUpdatePaused(const int&)));
        connect(this, SIGNAL(mockUpdateDownloaded()),
                m_instance, SLOT(handleUpdateDownloaded()));
        connect(this, SIGNAL(mockUpdateFailed(const int&, const QString&)),
                m_instance, SLOT(handleUpdateFailed(const int&, const QString&)));
        connect(this, SIGNAL(mockCurrentBuildNumberChanged()),
                m_instance, SLOT(handleCurrentBuildNumberChanged()));
        connect(this, SIGNAL(mockRebooting(const bool)),
                m_instance, SLOT(handleRebooting(const bool)));
        connect(this, SIGNAL(mockUpdateProcessing()),
                m_instance, SLOT(handleUpdateProcessing()));
        connect(this, SIGNAL(mockUpdateProcessFailed(const QString&)),
                m_instance, SLOT(handleUpdateProcessFailed(const QString&)));

    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
        delete m_model;
        delete m_mock;
        delete m_systemImage;
        delete m_dbus;
        delete m_siMock;
    }
    void testCheck()
    {
        m_instance->check();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "CheckForUpdate");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.
    }
    void testCancel()
    {
        m_instance->cancel();
        QVERIFY(m_methodSpy->wait());
        QDBusReply<QList<MethodCall>> reply = m_mock->call("GetMethodCalls", "CancelUpdate");
        QVERIFY2(reply.isValid(), "Method was not called correctly.");
        QCOMPARE(reply.value().size(), 1); // Called once.
    }
    void testUpdateAvailableStatus_data()
    {
        QTest::addColumn<bool>("isAvailable");
        QTest::addColumn<bool>("downloading");
        QTest::addColumn<QString>("availableVersion");
        QTest::addColumn<QString>("errorReason");
        QTest::addColumn<int>("downloadMode");
        QTest::addColumn<Update::State>("targetState");

        QTest::newRow("update") << true << false << "50" << "" << 0 << Update::State::StateAvailable;
        QTest::newRow("no update") << false << false << "50" << "" << 0 << Update::State::StateAvailable;
        QTest::newRow("downloading") << true << true << "50" << "" << 0 << Update::State::StateDownloading;
        QTest::newRow("paused") << true << false << "50" << "paused" << 0 << Update::State::StateDownloadPaused;
        QTest::newRow("(auto) downloading") << true << true << "50" << "" << 1 << Update::State::StateDownloadingAutomatically;
        QTest::newRow("(auto) paused") << true << false << "50" << "paused" << 1 << Update::State::StateAutomaticDownloadPaused;
    }
    void testUpdateAvailableStatus()
    {
        QFETCH(bool, isAvailable);
        QFETCH(bool, downloading);
        QFETCH(QString, availableVersion);
        QFETCH(QString, errorReason);
        QFETCH(int, downloadMode);
        QFETCH(Update::State, targetState);

        // Decides whether or not a download is automatic.
        m_systemImage->setDownloadMode(downloadMode);

        Q_EMIT mockUpdateAvailableStatus(isAvailable, downloading,
                                         availableVersion, 0, "", errorReason);

        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 50);
        if (isAvailable) {
            QVERIFY(!u.isNull());
        }

        if (downloading) {
            QCOMPARE(u->identifier(), Image::ManagerImpl::ubuntuId);
            QCOMPARE(u->state(), targetState);
        }
    }
    void testDownloadStarted()
    {
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 2, 0);
        Q_EMIT mockDownloadStarted();
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 2);
        QCOMPARE(u->state(), Update::State::StateDownloading);
    }
    void testUpdateProgress()
    {
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 2, 0);
        Q_EMIT mockUpdateProgress(50, 0);
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 2);
        QCOMPARE(u->state(), Update::State::StateDownloading);
        QCOMPARE(u->progress(), 50);
    }
    void testUpdatePaused()
    {
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 2, 0);
        Q_EMIT mockUpdatePaused(50);
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 2);
        QCOMPARE(u->state(), Update::State::StateDownloadPaused);
        QCOMPARE(u->progress(), 50);
    }
    void testUpdateDownloaded()
    {
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 2, 0);
        Q_EMIT mockUpdateDownloaded();
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 2);
        QCOMPARE(u->state(), Update::State::StateDownloaded);
    }
    void testUpdateFailed()
    {
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 2, 0);
        Q_EMIT mockUpdateFailed(1, "failure");
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 2);
        QCOMPARE(u->error(), QString("failure"));
    }
    void testCurrentBuildNumberChanged()
    {
        /* Makes sure any updates that has the current build number are marked
        as installed. */
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 1, 0);
        Q_EMIT mockCurrentBuildNumberChanged();
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 1);
        QVERIFY(u->installed());
    }
    void testRebooting_data()
    {
        /* status: A boolean indicating whether the device has initiated a reboot
        sequence or not. */
        QTest::addColumn<bool>("status");
        QTest::newRow("initiated") << true;
        QTest::newRow("not initiated") << false;
    }
    void testRebooting()
    {
        QFETCH(bool, status);
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 2, 0);
        Q_EMIT mockRebooting(status);
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 2);
        if (status) {
            QCOMPARE(u->state(), Update::State::StateInstalling);
        } else {
            QCOMPARE(u->state(), Update::State::StateFailed);
        }
    }
    void testUpdateProcessing()
    {
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 2, 0);
        Q_EMIT mockUpdateProcessing();
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 2);
        QCOMPARE(u->state(), Update::State::StateInstalling);
    }
    void testUpdateProcessFailed()
    {
        m_model->setImageUpdate(Image::ManagerImpl::ubuntuId, 2, 0);
        Q_EMIT mockUpdateProcessFailed("failure");
        QSharedPointer<Update> u = m_model->fetch(Image::ManagerImpl::ubuntuId, 2);
        QCOMPARE(u->state(), Update::State::StateFailed);
        QCOMPARE(u->error(), QString("failure"));
    }
Q_SIGNALS:
    void mockUpdateAvailableStatus(const bool isAvailable,
                                   const bool downloading,
                                   const QString &availableVersion,
                                   const int &updateSize,
                                   const QString &lastUpdateDate,
                                   const QString &errorReason);
    void mockDownloadStarted();
    void mockUpdateProgress(const int &percentage, const double &eta);
    void mockUpdatePaused(const int &percentage);
    void mockUpdateDownloaded();
    void mockUpdateFailed(const int &consecutiveFailureCount, const QString &lastReason);
    void mockCurrentBuildNumberChanged();
    void mockRebooting(const bool status);
    void mockUpdateProcessing();
    void mockUpdateProcessFailed(const QString &reason);
    void mockCheckingForUpdatesChanged();

private:
    QSignalSpy *m_methodSpy;
    FakeSystemImageDbus *m_siMock = nullptr;
    QDBusConnection *m_dbus = nullptr;
    QSystemImage *m_systemImage = nullptr;
    QDBusInterface *m_mock = nullptr;
    Image::Manager *m_instance = nullptr;
    UpdateModel *m_model = nullptr;
};

QTEST_GUILESS_MAIN(TstImageManager)
#include "tst_imagemanager.moc"
