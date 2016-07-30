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

#include <QDir>
#include <QJsonArray>
#include <QJsonParseError>
#include <QSignalSpy>
#include <QSqlError>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTest>

using namespace UpdatePlugin;

typedef QList<QSharedPointer<Update>> appList;

Q_DECLARE_METATYPE(appList)

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
        m_instance = new Click::ManagerImpl(m_model, nullptr, m_mockclient,
                                            m_mockmanifest, m_mocksso,
                                            m_mockdownloadfactory);
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
    QSharedPointer<Update> createUpdate(const QString &id, const uint &rev,
                                        const QString &version = "")
    {
        auto update = QSharedPointer<Update>(new Update);
        update->setIdentifier(id);
        update->setRevision(rev);
        update->setRemoteVersion(version);
        return update;
    }
    void testCheckStarts()
    {
        QSignalSpy checkStartsSpy(m_instance, SIGNAL(checkingForUpdatesChanged()));
        m_instance->check();
        QTRY_COMPARE(checkStartsSpy.count(), 1);
    }
    void testCheckAskForManifest()
    {
        m_instance->check();
        QVERIFY(m_mockmanifest->asked);
    }
    void testManifestUpload_data()
    {
        QTest::addColumn<QJsonArray>("manifest");
        QTest::addColumn<appList>("existingUpdates");
        {
            QByteArray manifest("[]");
            QTest::newRow("Empty") << JSONfromQByteArray(manifest) << appList();
        }
        {
            QByteArray manifest("[]");
            auto apps = appList();
            for (int i = 0; i < 20; i++) {
                apps.append(createUpdate(QString::number(i), i));
            }
            QTest::newRow("Empty manifest, tons of apps") << JSONfromQByteArray(manifest) << apps;
        }
        {
            QByteArray manifest("[{\"name\":\"a\", \"version\": \"1\"}]");
            auto apps = appList();
            apps << createUpdate("a", 0, "2");
            QTest::newRow("One") << JSONfromQByteArray(manifest) << apps;
        }
        {
            QByteArray manifest("["
                "{\"name\":\"a\", \"version\": \"1\"},"
                "{\"name\": \"b\", \"version\": \"1\"}"
            "]");
            auto apps = appList();
            apps << createUpdate("a", 0, "2") << createUpdate("b", 1, "2");
            QTest::newRow("Two") << JSONfromQByteArray(manifest) << apps;
        }
    }
    void testManifestUpload()
    {
        QFETCH(QJsonArray, manifest);
        QFETCH(appList, existingUpdates);

        Q_FOREACH(auto update, existingUpdates) {
            qWarning() << "adding existingUpdates" << update->identifier() << update->revision();
            m_model->add(update);
        }

        m_instance->check();
        m_mockmanifest->mockSuccess(manifest);

        /* We want to make sure only packages that we want to update was
        passed on to the click client. */
        QTRY_COMPARE(m_mockclient->requestedPackages.size(), manifest.size());
    }
    void testSynchronization_data()
    {
        QTest::addColumn<QJsonArray>("manifest");
        QTest::addColumn<appList>("existingUpdates");
        QTest::addColumn<appList>("markedInstalled");
        QTest::addColumn<appList>("removed");
        QTest::addColumn<appList>("targetDbUpdates");

        {   // Mark one update as installed.
            QByteArray manifest("[{\"name\": \"a\", \"version\": \"v1\"}]");
            appList existing, installed, removed, targetDbUpdates;
            auto package1 = createUpdate("a", 0, "v1");
            existing << package1;
            installed << package1;
            QTest::newRow("One installed")
                << JSONfromQByteArray(manifest) << existing << installed
                << removed << targetDbUpdates;
        }
        {   // Remove an update.
            QByteArray manifest("[]");
            appList existing, installed, removed, targetDbUpdates;
            auto package1 = createUpdate("a", 0, "v1");
            existing << package1;
            removed << package1;
            QTest::newRow("One removed")
                << JSONfromQByteArray(manifest) << existing << installed
                << removed << targetDbUpdates;
        }
        {   // Mark two updates as installed.
            QByteArray manifest("["
                "{\"name\": \"b\", \"version\": \"v1\" },"
                "{\"name\": \"c\", \"version\": \"v2\" }"
            "]");
            appList existing, installed, removed, targetDbUpdates;
            auto package1 = createUpdate("b", 0, "v1");
            auto package2 = createUpdate("c", 0, "v2");
            existing << package1 << package2;
            QTest::newRow("Two installed")
                << JSONfromQByteArray(manifest) << existing << installed
                << removed << targetDbUpdates;
        }
        {   // Remove two updates.
            QByteArray manifest("[]");
            appList existing, installed, removed, targetDbUpdates;
            auto package1 = createUpdate("b", 0, "v1");
            auto package2 = createUpdate("c", 0, "v2");
            removed << package1 << package2;
            QTest::newRow("Two removed")
                << JSONfromQByteArray(manifest) << existing << installed
                << removed << targetDbUpdates;
        }
        {   // Remove one, mark one as installed.
            QByteArray manifest("[{\"name\": \"a\", \"version\": \"v1\"}]");
            appList existing, installed, removed, targetDbUpdates;
            auto package1 = createUpdate("a", 0, "v1");
            auto package2 = createUpdate("b", 0, "v2");
            existing << package1 << package2;
            installed << package1;
            removed << package2;
            QTest::newRow("Mix")
                << JSONfromQByteArray(manifest) << existing << installed
                << removed << targetDbUpdates;
        }
        {   // No changes to the two updates.
            QByteArray manifest("["
                "{\"name\": \"a\", \"version\": \"v1\" },"
                "{\"name\": \"b\", \"version\": \"v1\" }"
            "]");
            appList existing, installed, removed, targetDbUpdates;
            auto package1 = createUpdate("a", 0, "v2");
            auto package2 = createUpdate("b", 0, "v2");
            existing << package1 << package2;
            targetDbUpdates << package1 << package2;
            QTest::newRow("No change")
                << JSONfromQByteArray(manifest) << existing << installed
                << removed << targetDbUpdates;
        }
    }
    void testSynchronization()
    {
        /* At start-up, we want to test the manifest against our database,
        independent of whether or not a check should be started. This test
        ensures that 1) a check is not started, 2) our database is
        synchronized. */
        QFETCH(QJsonArray, manifest);
        QFETCH(appList, existingUpdates);
        QFETCH(appList, markedInstalled);
        QFETCH(appList, removed);
        QFETCH(appList, targetDbUpdates);

        Q_FOREACH(auto update, existingUpdates) {
            m_model->add(update);
        }

        m_mockmanifest->mockSuccess(manifest);

        Q_FOREACH(auto update, markedInstalled) {
            QVERIFY(m_model->get(update)->installed());
        }
        Q_FOREACH(auto update, removed) {
            QVERIFY(m_model->get(update).isNull());
        }
        Q_FOREACH(auto update, targetDbUpdates) {
            QVERIFY(!m_model->fetch(update).isNull());
        }

        // Assert no client interaction after synchronizing.
        QVERIFY(m_mockclient->requestedUrl.isEmpty());
    }
    void testManifestFailure()
    {
        m_instance->check();
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));
        m_mockmanifest->mockFailure();
        QTRY_COMPARE(checkCompletedSpy.count(), 1);
    }
    void testClientNetworkErrorAbortsCheck()
    {
        m_instance->check();
        QSignalSpy networkErrorSpy(m_instance, SIGNAL(networkError()));
        QSignalSpy checkCanceledSpy(m_instance, SIGNAL(checkCanceled()));
        m_mockclient->mockNetworkError();
        QTRY_COMPARE(checkCanceledSpy.count(), 1);
        QTRY_COMPARE(networkErrorSpy.count(), 1);
    }
    void testClientServerErrorAbortsCheck()
    {
        m_instance->check();
        QSignalSpy serverErrorSpy(m_instance, SIGNAL(serverError()));
        QSignalSpy checkCanceledSpy(m_instance, SIGNAL(checkCanceled()));
        m_mockclient->mockServerError();
        QTRY_COMPARE(checkCanceledSpy.count(), 1);
        QTRY_COMPARE(serverErrorSpy.count(), 1);
    }
    void testClientCredentialErrorAbortsCheck()
    {
        m_instance->check();
        QSignalSpy credentialErrorSpy(m_instance, SIGNAL(credentialError()));
        QSignalSpy checkCanceledSpy(m_instance, SIGNAL(checkCanceled()));
        QSignalSpy authenticatedChangedSpy(m_instance, SIGNAL(authenticatedChanged()));
        m_mockclient->mockCredentialError();
        QTRY_COMPARE(checkCanceledSpy.count(), 1);
        QTRY_COMPARE(credentialErrorSpy.count(), 1);

        // This should also de authenticate the user.
        QTRY_COMPARE(authenticatedChangedSpy.count(), 1);
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
    void testSSOCredentialsNotFound()
    {
        QSignalSpy authenticatedChangedSpy(m_instance, SIGNAL(authenticatedChanged()));
        m_mocksso->mockCredentialsNotFound();
        QTRY_COMPARE(authenticatedChangedSpy.count(), 1);
    }
    void testSSOCredentialsDeleted()
    {
        QSignalSpy authenticatedChangedSpy(m_instance, SIGNAL(authenticatedChanged()));
        m_mocksso->mockCredentialsDeleted();
        QTRY_COMPARE(authenticatedChangedSpy.count(), 1);
    }
    void testSSOCredentialsFound()
    {
        QSignalSpy authenticatedChangedSpy(m_instance, SIGNAL(authenticatedChanged()));
        m_mocksso->mockCredentialsDeleted();
        QTRY_COMPARE(authenticatedChangedSpy.count(), 1);

        // The token is ignored.
        m_mocksso->mockCredentialsFound(UbuntuOne::Token());
        QTRY_COMPARE(authenticatedChangedSpy.count(), 2);
    }
    void testManifestParser()
    {
        QByteArray manifest("[{"
            "\"name\": \"a\","
            "\"version\": \"0\","
            "\"hooks\": {"
            "    \"A\": {},"
            "    \"B\": {\"desktop\": \"\"}"
            "}"
        "}]");

        QByteArray metadata("[{"
            "\"name\": \"a\","
            "\"version\": \"1\","
            "\"revision\": \"1\""
        "}]");

        m_instance->check();

        // Transition the manifest data all the way to the model.
        m_mockmanifest->mockSuccess(JSONfromQByteArray(manifest));
        m_mockclient->mockMetadataRequestSucceeded(JSONfromQByteArray(metadata));
        auto dl = m_mockdownloadfactory->created.at(0);
        dl->mockDownloadSucceeded("token");

        // Update now in model, assert that the manifest data has been captured.
        auto u = m_model->get("a", 0);
        QCOMPARE(u->identifier(), QString("a"));
        QCOMPARE(u->localVersion(), QString("0"));
        QCOMPARE(u->packageName(), QString("B"));
    }
    void testRemovedApp()
    {
        /* Tests that an app that was removed from the manifest, that is still
        in the update db, will be removed from the db. */
        m_model->add(createUpdate("a", 1));

        // a.1 is not in the manifest, so it should be removed.
        QByteArray manifest("[]");
        m_mockmanifest->mockSuccess(JSONfromQByteArray(manifest));
        QVERIFY(m_model->get("a", 1).isNull());
    }
    void testTokenDownload_data()
    {
        QTest::addColumn<QJsonArray>("metadata");
        QTest::addColumn<QJsonArray>("manifest");
        QTest::addColumn<bool>("downloadSuccess");
        QTest::addColumn<QString>("downloadedToken");

        QByteArray metaJson("[{"
            "\"name\": \"a\","
            "\"version\": \"1\", \"download_url\": \"download_url\""
        "}]");
        auto metadata = JSONfromQByteArray(metaJson);

        QByteArray manifestJson("[{"
            "\"name\": \"a\","
            "\"version\": \"0\""
        "}]");
        auto manifest = JSONfromQByteArray(manifestJson);

        {
            QString token("token");
            bool success = true;
            QTest::newRow("Success")
                << metadata << manifest << success << token;
        }
        {
            QString token("");
            bool success = true;
            QTest::newRow("Success (empty token)")
                << metadata << manifest << success << token;
        }
        {
            QString token("");
            bool success = false;
            QTest::newRow("Failure")
                << metadata << manifest << success << token;
        }
    }
    void testTokenDownload()
    {
        QFETCH(QJsonArray, metadata);
        QFETCH(QJsonArray, manifest);
        QFETCH(bool, downloadSuccess);
        QFETCH(QString, downloadedToken);

        m_instance->check();
        m_mockmanifest->mockSuccess(manifest);
        m_mockclient->mockMetadataRequestSucceeded(metadata);

        QTRY_COMPARE(m_mockdownloadfactory->created.size(), 1);
        auto *dl = m_mockdownloadfactory->created.at(0);
        QCOMPARE(dl->downloadUrl, QString("download_url"));

        QSignalSpy checkCompletesSpy(m_instance, SIGNAL(checkCompleted()));
        if (downloadSuccess) {
            dl->mockDownloadSucceeded(downloadedToken);
        } else {
            dl->mockDownloadFailed();
        }
        QTRY_COMPARE(checkCompletesSpy.count(), 1);
    }
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
    void testRemotelyUpdatedApp()
    {
        /* Tests that apps that are remotely updated, get marked as such. */

        auto update = createUpdate("a", 0);
        update->setRemoteVersion("v1");
        m_model->add(update);

        QByteArray manifest("[{"
            "\"name\": \"a\","
            "\"version\": \"v1\""
        "}]");

        // The manifest returns that the update is installed.
        m_mockmanifest->mockSuccess(JSONfromQByteArray(manifest));

        QVERIFY(m_model->get("a", 0)->installed());

        // Assert no client interaction while not checking.
        QVERIFY(m_mockclient->requestedUrl.isEmpty());
    }
private:
    // Create JSON Array from a QByteArray.
    QJsonArray JSONfromQByteArray(const QByteArray &byteArray)
    {
        QJsonArray ret;
        auto jsonError = new QJsonParseError;
        auto document = QJsonDocument::fromJson(byteArray, jsonError);

        if (document.isArray()) {
            ret = document.array();
        }

        if (jsonError->error != QJsonParseError::NoError) {
            qWarning() << Q_FUNC_INFO  << "Could not parse json:"
                       << jsonError->errorString() << ", data: "
                       << byteArray;
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
#include "tst_clickmanager.moc"
