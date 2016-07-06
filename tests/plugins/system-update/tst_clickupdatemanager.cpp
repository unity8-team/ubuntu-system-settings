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
#include "helpers.h"
#include "clickupdatemanager.h"

#include "fakeclient.h"
#include "fakemanifest.h"
#include "fakesso.h"
#include "faketokendownloader.h"
#include "faketokendownloader_factory.h"

#include <QSignalSpy>
#include <QSqlError>
#include <QTemporaryDir>
#include <QSqlQuery>
#include <QTest>

using namespace UpdatePlugin;

class TstClickUpdateManager : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_db = new UpdateDb(m_dir->path() + "/cupdatemanagerstore.db");

        m_mockclient = new MockClient;
        m_mockmanifest = new MockManifest;
        m_mocksso = new MockSSO;
        m_mockdownloadfactory = new MockTokenDownloaderFactory;

        m_instance = new ClickUpdateManager(m_mockclient, m_mockmanifest, m_mocksso,
                                            m_mockdownloadfactory, m_db);

        m_mockclient->setParent(m_instance);
        m_mockmanifest->setParent(m_instance);
        m_mocksso->setParent(m_instance);
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
        delete m_dir;

    }
    void testCheckStarts()
    {
        QSignalSpy checkStartsSpy(m_instance, SIGNAL(checkStarted()));
        m_instance->check();
        QTRY_COMPARE(checkStartsSpy.count(), 1);
    }
    void testCheckAskForManifest()
    {
        m_instance->check();
        QTRY_COMPARE(m_mockmanifest->asked, true);
    }
    void testManifestSuccess_data()
    {
        QTest::addColumn<QJsonArray>("manifest");

        QStringList empty;
        QTest::newRow("Empty") << QJsonArray::fromStringList(empty);

        QStringList one; one << "package";
        QTest::newRow("One") << QJsonArray::fromStringList(one);

        QStringList two; two << "package1" << "package2";
        QTest::newRow("Two") << QJsonArray::fromStringList(one);
    }
    void testManifestSuccess()
    {
        QFETCH(QJsonArray, manifest);
        m_mockmanifest->mockSuccess(manifest);
        QTRY_COMPARE(m_mockclient->requestedPackages.size(), manifest.size());

        if (manifest.size())
            QTRY_COMPARE(m_mockclient->requestedUrl.toString(), Helpers::clickMetadataUrl());

    }
    void testManifestFailureWhileChecking()
    {
        m_instance->check();
        QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
        m_mockmanifest->mockFailure();
        QTRY_COMPARE(checkFailedSpy.count(), 1);
    }
    void testClientNetworkErrorAbortsCheck()
    {
        m_instance->check();
        QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
        m_mockclient->mockNetworkError();
        QTRY_COMPARE(checkFailedSpy.count(), 1);
    }
    void testClientServerErrorAbortsCheck()
    {
        m_instance->check();
        QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
        m_mockclient->mockServerError();
        QTRY_COMPARE(checkFailedSpy.count(), 1);
    }
    void testClientCredentialErrorAbortsCheck()
    {
        m_instance->check();
        QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
        m_mockclient->mockCredentialError();
        QTRY_COMPARE(checkFailedSpy.count(), 1);
    }
    void testClientSignalForwarding()
    {
        QSignalSpy networkErrorSpy(m_instance, SIGNAL(networkError()));
        m_mockclient->mockNetworkError();
        QTRY_COMPARE(networkErrorSpy.count(), 1);

        QSignalSpy serverErrorSpy(m_instance, SIGNAL(serverError()));
        m_mockclient->mockServerError();
        QTRY_COMPARE(serverErrorSpy.count(), 1);

        QSignalSpy credentialErrorSpy(m_instance, SIGNAL(credentialError()));
        m_mockclient->mockCredentialError();
        QTRY_COMPARE(credentialErrorSpy.count(), 1);
    }
    void testSSOCredentialFailure()
    {
        QSignalSpy authenticatedChangedSpy(m_instance, SIGNAL(authenticatedChanged()));
        m_mocksso->mockCredentialsRequestFailed();
        QTRY_COMPARE(authenticatedChangedSpy.count(), 1);
    }
    void testSSOCredentialSuccess()
    {
        QSignalSpy authenticatedChangedSpy(m_instance, SIGNAL(authenticatedChanged()));
        m_mocksso->mockCredentialsRequestFailed();
        QTRY_COMPARE(authenticatedChangedSpy.count(), 1);

        // The token is ignored.
        m_mocksso->mockCredentialsRequestSucceeded(UbuntuOne::Token());
        QTRY_COMPARE(authenticatedChangedSpy.count(), 2);
    }
    void testTokenDownload_data()
    {
        QTest::addColumn<QByteArray>("metadata");
        QTest::addColumn<QJsonArray>("manifest");
        QTest::addColumn<bool>("downloadSuccess");
        QTest::addColumn<QString>("downloadedToken");

        QByteArray onePackage("[{"
            "\"name\": \"in_need_of_update\","
            "\"version\": \"1\","
            "\"download_url\": \"download_url\""
        "}]");

        QList<QVariant> manifest;
            QVariantMap onePackageManifest;
            onePackageManifest["name"] = "in_need_of_update";
            onePackageManifest["version"] = "0";
        manifest << onePackageManifest;

        QTest::newRow("Success") << onePackage << QJsonArray::fromVariantList(manifest) << true << "token";
        QTest::newRow("Success (empty token)") << onePackage << QJsonArray::fromVariantList(manifest) << true << "";
        QTest::newRow("Failure") << onePackage << QJsonArray::fromVariantList(manifest) << false << "";
    }
    void testTokenDownload()
    {
        QFETCH(QByteArray, metadata);
        QFETCH(QJsonArray, manifest);
        QFETCH(bool, downloadSuccess);
        QFETCH(QString, downloadedToken);

        m_mockmanifest->mockSuccess(manifest);
        m_mockclient->mockMetadataRequestSucceeded(metadata);

        QTRY_COMPARE(m_mockdownloadfactory->created.size(), 1);
        MockTokenDownloader *dl = m_mockdownloadfactory->created.at(0);
        QCOMPARE(dl->downloadUrl, QString("download_url"));

        QSignalSpy checkCompletesSpy(m_instance, SIGNAL(checkCompleted()));
        if (downloadSuccess) {
            dl->mockDownloadSucceeded(downloadedToken);
        } else {
            dl->mockDownloadFailed();
        }
        QTRY_COMPARE(checkCompletesSpy.count(), 1);
    }
    void testRetryUpdate()
    {
        QString id("package1");
        int rev = 42;
        QString targetToken("foobar");

        QSharedPointer<Update> u = QSharedPointer<Update>(new Update);
        u->setIdentifier(id);
        u->setRevision(rev);
        m_db->add(u);

        m_instance->check(id, rev);
        MockTokenDownloader *dl = m_mockdownloadfactory->created.at(0);
        dl->mockDownloadSucceeded("foobar");

        QCOMPARE(m_db->get(id, rev)->token(), QString("foobar"));
    }
    void testCheckRequired()
    {
        QSqlQuery q(m_db->db());
        QDateTime longAgo = QDateTime::currentDateTime().addMonths(-1).addDays(-1).toUTC();

        q.prepare("REPLACE INTO meta (checked_at_utc) VALUES (:checked_at_utc)");
        q.bindValue(":checked_at_utc", longAgo.toMSecsSinceEpoch());

        QVERIFY(q.exec());

        QVERIFY(m_instance->isCheckRequired());
    }
    void testNotRequiredAfterCheck()
    {
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));
        m_instance->check();

        // Empty manifest will complete check.
        m_mockmanifest->mockSuccess(QJsonArray::fromStringList(QStringList()));

        QTRY_COMPARE(checkCompletedSpy.count(), 1);
        QVERIFY(!m_instance->isCheckRequired());
    }
private:
    MockClient *m_mockclient = nullptr;
    MockManifest *m_mockmanifest = nullptr;
    MockSSO *m_mocksso = nullptr;
    MockTokenDownloaderFactory *m_mockdownloadfactory = nullptr;

    ClickUpdateManager *m_instance = nullptr;
    UpdateDb *m_db = nullptr;
    QTemporaryDir *m_dir;
};

QTEST_MAIN(TstClickUpdateManager)
#include "tst_clickupdatemanager.moc"
