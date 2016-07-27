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
#include "click/manager_impl.h"

#include "plugins/system-update/fakeclient.h"
#include "plugins/system-update/fakemanifest.h"
#include "plugins/system-update/fakesso.h"
#include "plugins/system-update/faketokendownloader.h"
#include "plugins/system-update/faketokendownloader_factory.h"

#include <QJsonArray>
#include <QJsonParseError>
#include <QSignalSpy>
#include <QSqlError>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTest>

using namespace UpdatePlugin;

class TstClickManager : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_model = new UpdateModel(m_dir->path() + "/cupdatemanagerstore.db");

        m_mockclient = new MockClient;
        m_mockmanifest = new MockManifest;
        m_mocksso = new MockSSO;
        m_mockdownloadfactory = new MockTokenDownloaderFactory;

        m_instance = new Click::ManagerImpl(m_mockclient, m_mockmanifest,
                                            m_mocksso, m_mockdownloadfactory,
                                            m_model);
        connect(this, SIGNAL(mockCheck()),
                m_instance, SLOT(handleCheckStart()));
        connect(this, SIGNAL(mockCancelCheck()),
                m_instance, SLOT(handleCheckStop()));

        m_model->setParent(m_instance);
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
    // void testCheckAskForManifest()
    // {
    //     m_instance->check();
    //     QTRY_COMPARE(m_mockmanifest->asked, true);
    // }
    // void testManifestSuccessWhileChecking_data()
    // {
    //     QTest::addColumn<QJsonArray>("manifest");

    //     QStringList empty;
    //     QTest::newRow("Empty") << QJsonArray::fromStringList(empty);

    //     QStringList one; one << "package";
    //     QTest::newRow("One") << QJsonArray::fromStringList(one);

    //     QStringList two; two << "package1" << "package2";
    //     QTest::newRow("Two") << QJsonArray::fromStringList(one);
    // }
    // void testManifestSuccessWhileChecking()
    // {
    //     QFETCH(QJsonArray, manifest);

    //     Q_EMIT mockCheck();
    //     m_mockmanifest->mockSuccess(manifest);
    //     QTRY_COMPARE(m_mockclient->requestedPackages.size(), manifest.size());
    //     if (manifest.size())
    //         QTRY_COMPARE(m_mockclient->requestedUrl.toString(), Helpers::clickMetadataUrl());
    // }
    // void testManifestSuccessOutsideCheck_data()
    // {
    //     QTest::addColumn<QJsonArray>("manifest");
    //     QTest::addColumn<QList<QSharedPointer<Update>>>("existingUpdates");

    //     QByteArray manifestA("[{"
    //         "\"name\": \"a\","
    //         "\"version\": \"v1\""
    //     "}]");
    //     QList<QSharedPointer<Update>> updatesA;
    //     QSharedPointer<Update> packageA = QSharedPointer<Update>(new Update);
    //     packageA->setIdentifier("a"); packageA->setRevision(0);
    //     packageA->setRemoteVersion("v1");
    //     updatesA << packageA;
    //     QTest::newRow("One") << JSONfromQByteArray(manifestA) << updatesA;

    //     QByteArray manifestB("["
    //         "{\"name\": \"b\", \"version\": \"v1\" },"
    //         "{\"name\": \"c\", \"version\": \"v2\" }"
    //     "]");
    //     QList<QSharedPointer<Update>> updatesB;

    //     QSharedPointer<Update> packageB = QSharedPointer<Update>(new Update);
    //     packageB->setIdentifier("b"); packageB->setRevision(0);
    //     packageB->setRemoteVersion("v1");

    //     QSharedPointer<Update> packageC = QSharedPointer<Update>(new Update);
    //     packageC->setIdentifier("c"); packageC->setRevision(0);
    //     packageC->setRemoteVersion("v2");

    //     updatesB << packageB << packageC;
    //     QTest::newRow("Two") << JSONfromQByteArray(manifestB) << updatesB;
    // }
    // void testManifestSuccessOutsideCheck()
    // {
    //     QFETCH(QJsonArray, manifest);
    //     QFETCH(QList<QSharedPointer<Update>>, existingUpdates);

    //     Q_FOREACH(const QSharedPointer<Update> &update, existingUpdates) {
    //         m_model->add(update);
    //     }

    //     m_mockmanifest->mockSuccess(manifest);

    //     // Existing updates should now be marked as installed.
    //     Q_FOREACH(const QSharedPointer<Update> &update, existingUpdates) {
    //         QVERIFY(m_model->get(update->identifier(), update->revision())->installed());
    //     }

    //     // Assert no client interaction while not checking.
    //     QVERIFY(m_mockclient->requestedUrl.isEmpty());
    // }
    // void testManifestFailureWhileChecking()
    // {
    //     m_instance->check();
    //     QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
    //     m_mockmanifest->mockFailure();
    //     QTRY_COMPARE(checkFailedSpy.count(), 1);
    // }
    // void testClientNetworkErrorAbortsCheck()
    // {
    //     m_instance->check();
    //     QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
    //     m_mockclient->mockNetworkError();
    //     QTRY_COMPARE(checkFailedSpy.count(), 1);
    // }
    // void testClientServerErrorAbortsCheck()
    // {
    //     m_instance->check();
    //     QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
    //     m_mockclient->mockServerError();
    //     QTRY_COMPARE(checkFailedSpy.count(), 1);
    // }
    // void testClientCredentialErrorAbortsCheck()
    // {
    //     m_instance->check();
    //     QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
    //     m_mockclient->mockCredentialError();
    //     QTRY_COMPARE(checkFailedSpy.count(), 1);
    // }
    // void testClientSignalForwarding()
    // {
    //     QSignalSpy networkErrorSpy(m_instance, SIGNAL(networkError()));
    //     m_mockclient->mockNetworkError();
    //     QTRY_COMPARE(networkErrorSpy.count(), 1);

    //     QSignalSpy serverErrorSpy(m_instance, SIGNAL(serverError()));
    //     m_mockclient->mockServerError();
    //     QTRY_COMPARE(serverErrorSpy.count(), 1);

    //     QSignalSpy credentialErrorSpy(m_instance, SIGNAL(credentialError()));
    //     m_mockclient->mockCredentialError();
    //     QTRY_COMPARE(credentialErrorSpy.count(), 1);
    // }
    // void testSSOCredentialFailure()
    // {
    //     QSignalSpy authenticatedChangedSpy(m_instance, SIGNAL(authenticatedChanged()));
    //     m_mocksso->mockCredentialsRequestFailed();
    //     QTRY_COMPARE(authenticatedChangedSpy.count(), 1);
    // }
    // void testSSOCredentialSuccess()
    // {
    //     QSignalSpy authenticatedChangedSpy(m_instance, SIGNAL(authenticatedChanged()));
    //     m_mocksso->mockCredentialsRequestFailed();
    //     QTRY_COMPARE(authenticatedChangedSpy.count(), 1);

    //     // The token is ignored.
    //     m_mocksso->mockCredentialsRequestSucceeded(UbuntuOne::Token());
    //     QTRY_COMPARE(authenticatedChangedSpy.count(), 2);
    // }
    // void testManifestParser()
    // {
    //     QByteArray manifest("[{"
    //         "\"name\": \"a\","
    //         "\"version\": \"0\","
    //         "\"hooks\": {"
    //         "    \"A\": {},"
    //         "    \"B\": {\"desktop\": \"\"}"
    //         "}"
    //     "}]");

    //     QByteArray metadata("[{"
    //         "\"name\": \"a\","
    //         "\"version\": \"1\","
    //         "\"download_url\": \"download_url\""
    //     "}]");

    //     Q_EMIT mockCheck();

    //     // Transition the manifest data all the way to the model.
    //     m_mockmanifest->mockSuccess(JSONfromQByteArray(manifest));
    //     m_mockclient->mockMetadataRequestSucceeded(JSONfromQByteArray(metadata));
    //     QTRY_COMPARE(m_mockdownloadfactory->created.size(), 1);
    //     MockTokenDownloader *dl = m_mockdownloadfactory->created.at(0);
    //     dl->mockDownloadSucceeded("token");

    //     // Update now in model, assert that the manifest data has been captured.
    //     QSharedPointer<Update> u = m_model->get("a", 0);
    //     QCOMPARE(u->identifier(), QString("a"));
    //     QCOMPARE(u->localVersion(), QString("0"));
    //     QCOMPARE(u->packageName(), QString("B"));
    // }
    // void testTokenDownload_data()
    // {
    //     QTest::addColumn<QJsonArray>("metadata");
    //     QTest::addColumn<QJsonArray>("manifest");
    //     QTest::addColumn<bool>("downloadSuccess");
    //     QTest::addColumn<QString>("downloadedToken");

    //     QByteArray onePackage("[{"
    //         "\"name\": \"in_need_of_update\","
    //         "\"version\": \"1\","
    //         "\"download_url\": \"download_url\""
    //     "}]");


    //     QByteArray manifest("[{"
    //         "\"name\": \"in_need_of_update\","
    //         "\"version\": \"0\""
    //     "}]");

    //     QTest::newRow("Success")
    //         << JSONfromQByteArray(onePackage)
    //         << JSONfromQByteArray(manifest)
    //         << true
    //         << "token";

    //     QTest::newRow("Success (empty token)")
    //         << JSONfromQByteArray(onePackage)
    //         << JSONfromQByteArray(manifest)
    //         << true
    //         << "";
    //     QTest::newRow("Failure")
    //         << JSONfromQByteArray(onePackage)
    //         << JSONfromQByteArray(manifest)
    //         << false
    //         << "";
    // }
    // void testTokenDownload()
    // {
    //     QFETCH(QJsonArray, metadata);
    //     QFETCH(QJsonArray, manifest);
    //     QFETCH(bool, downloadSuccess);
    //     QFETCH(QString, downloadedToken);

    //     Q_EMIT mockCheck();

    //     m_mockmanifest->mockSuccess(manifest);
    //     m_mockclient->mockMetadataRequestSucceeded(metadata);

    //     QTRY_COMPARE(m_mockdownloadfactory->created.size(), 1);
    //     MockTokenDownloader *dl = m_mockdownloadfactory->created.at(0);
    //     QCOMPARE(dl->downloadUrl, QString("download_url"));

    //     QSignalSpy checkCompletesSpy(m_instance, SIGNAL(checkCompleted()));
    //     if (downloadSuccess) {
    //         dl->mockDownloadSucceeded(downloadedToken);
    //     } else {
    //         dl->mockDownloadFailed();
    //     }
    //     QTRY_COMPARE(checkCompletesSpy.count(), 1);
    // }
    // void testRetryUpdate()
    // {
    //     QString id("package1");
    //     int rev = 42;
    //     QString targetToken("foobar");

    //     QSharedPointer<Update> u = QSharedPointer<Update>(new Update);
    //     u->setIdentifier(id);
    //     u->setRevision(rev);
    //     m_model->add(u);

    //     m_instance->retry(id, rev);
    //     MockTokenDownloader *dl = m_mockdownloadfactory->created.at(0);
    //     dl->mockDownloadSucceeded("foobar");

    //     QCOMPARE(m_model->get(id, rev)->token(), QString("foobar"));
    // }
    // void testUseMetadataToUpdateState()
    // {
    //     // Add update.
    //     QSharedPointer<Update> u = QSharedPointer<Update>(new Update);
    //     u->setIdentifier("a");
    //     u->setRevision(0);
    //     u->setRemoteVersion("v1");
    //     m_model->add(u);

    //     QByteArray manifest("[{"
    //         "\"name\": \"a\","
    //         "\"version\": \"v1\""
    //     "}]");

    //     // The manifest returns that the update is installed.
    //     m_mockmanifest->mockSuccess(JSONfromQByteArray(manifest));

    //     QVERIFY(m_model->get("a", 0)->installed());
    // }

Q_SIGNALS:
    void mockCheck();
    void mockCancelCheck();

private:
    QJsonArray JSONfromQByteArray(const QByteArray &byteArray)
    {
        QJsonArray ret;
        QJsonParseError *jsonError = new QJsonParseError;
        auto document = QJsonDocument::fromJson(byteArray, jsonError);

        if (document.isArray()) {
            ret = document.array();
        }

        if (jsonError->error != QJsonParseError::NoError) {
            qWarning() << Q_FUNC_INFO  << "Could not parse json:"
                       << jsonError->errorString();
        }

        delete jsonError;
        return ret;
    }
    MockClient *m_mockclient = nullptr;
    MockManifest *m_mockmanifest = nullptr;
    MockSSO *m_mocksso = nullptr;
    MockTokenDownloaderFactory *m_mockdownloadfactory = nullptr;

    Click::Manager *m_instance = nullptr;
    UpdateModel *m_model = nullptr;
    QTemporaryDir *m_dir;
};

QTEST_MAIN(TstClickManager)
#include "tst_click_update_manager.moc"
