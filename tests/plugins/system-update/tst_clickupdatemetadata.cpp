#include <QSignalSpy>
#include <QTest>

#include "mockclickservertestcase.h"

#include "clickupdatemetadata.h"

class TstClickUpdateMetadata
    : public QObject
    , public MockClickServerTestCase
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        startMockClickServer();
        QSignalSpy readyReadStandardOutputSpy(&m_mockclickserver,
            SIGNAL(readyReadStandardOutput()));
        QVERIFY(readyReadStandardOutputSpy.wait());
    }
    void cleanupTestCase()
    {
        m_mockclickserver.close();
    }
    void init()
    {
        m_instance = new UpdatePlugin::ClickUpdateMetadata();
    }
    void cleanup()
    {
        delete m_instance;
    }
    void testProperties()
    {
        QStringList depts; depts << "foo" << "bar";

        QSignalSpy anonDownloadUrlSpy(m_instance, SIGNAL(anonDownloadUrlChanged()));
        m_instance->setAnonDownloadUrl("anonDownloadUrl");
        QCOMPARE(anonDownloadUrlSpy.count(), 1);
        QVERIFY(m_instance->anonDownloadUrl() == "anonDownloadUrl");

        QSignalSpy binaryFilesizeSpy(m_instance, SIGNAL(binaryFilesizeChanged()));
        m_instance->setBinaryFilesize(1000);
        QCOMPARE(binaryFilesizeSpy.count(), 1);
        QVERIFY(m_instance->binaryFilesize() == 1000);

        QSignalSpy changelogSpy(m_instance, SIGNAL(changelogChanged()));
        m_instance->setChangelog("changelog");
        QCOMPARE(changelogSpy.count(), 1);
        QVERIFY(m_instance->changelog() == "changelog");

        QSignalSpy channelSpy(m_instance, SIGNAL(channelChanged()));
        m_instance->setChannel("channel");
        QCOMPARE(channelSpy.count(), 1);
        QVERIFY(m_instance->channel() == "channel");

        QSignalSpy contentSpy(m_instance, SIGNAL(contentChanged()));
        m_instance->setContent("content");
        QCOMPARE(contentSpy.count(), 1);
        QVERIFY(m_instance->content() == "content");

        QSignalSpy departmentSpy(m_instance, SIGNAL(departmentChanged()));
        m_instance->setDepartment(depts);
        QCOMPARE(departmentSpy.count(), 1);
        QVERIFY(m_instance->department() == depts);

        QSignalSpy downloadSha512Spy(m_instance, SIGNAL(downloadSha512Changed()));
        m_instance->setDownloadSha512("downloadSha512");
        QCOMPARE(downloadSha512Spy.count(), 1);
        QVERIFY(m_instance->downloadSha512() == "downloadSha512");

        QSignalSpy downloadUrlSpy(m_instance, SIGNAL(downloadUrlChanged()));
        m_instance->setDownloadUrl("downloadUrl");
        QCOMPARE(downloadUrlSpy.count(), 1);
        QVERIFY(m_instance->downloadUrl() == "downloadUrl");

        QSignalSpy iconUrlSpy(m_instance, SIGNAL(iconUrlChanged()));
        m_instance->setIconUrl("iconUrl");
        QCOMPARE(iconUrlSpy.count(), 1);
        QVERIFY(m_instance->iconUrl() == "iconUrl");

        QSignalSpy nameSpy(m_instance, SIGNAL(nameChanged()));
        m_instance->setName("name");
        QCOMPARE(nameSpy.count(), 1);
        QVERIFY(m_instance->name() == "name");

        QSignalSpy originSpy(m_instance, SIGNAL(originChanged()));
        m_instance->setOrigin("origin");
        QCOMPARE(originSpy.count(), 1);
        QVERIFY(m_instance->origin() == "origin");

        QSignalSpy packageNameSpy(m_instance, SIGNAL(packageNameChanged()));
        m_instance->setPackageName("packageName");
        QCOMPARE(packageNameSpy.count(), 1);
        QVERIFY(m_instance->packageName() == "packageName");

        QSignalSpy revisionSpy(m_instance, SIGNAL(revisionChanged()));
        m_instance->setRevision(10);
        QCOMPARE(revisionSpy.count(), 1);
        QVERIFY(m_instance->revision() == 10);

        QSignalSpy statusSpy(m_instance, SIGNAL(statusChanged()));
        m_instance->setStatus("status");
        QCOMPARE(statusSpy.count(), 1);
        QVERIFY(m_instance->status() == "status");

        QSignalSpy titleSpy(m_instance, SIGNAL(titleChanged()));
        m_instance->setTitle("title");
        QCOMPARE(titleSpy.count(), 1);
        QVERIFY(m_instance->title() == "title");

        QSignalSpy remoteVersionSpy(m_instance, SIGNAL(remoteVersionChanged()));
        m_instance->setRemoteVersion("version");
        QCOMPARE(remoteVersionSpy.count(), 1);
        QVERIFY(m_instance->remoteVersion() == "version");

        QSignalSpy localVersionSpy(m_instance, SIGNAL(localVersionChanged()));
        m_instance->setLocalVersion("version");
        QCOMPARE(localVersionSpy.count(), 1);
        QVERIFY(m_instance->localVersion() == "version");

        QSignalSpy clickTokenSpy(m_instance, SIGNAL(clickTokenChanged()));
        m_instance->setClickToken("clickToken");
        QCOMPARE(clickTokenSpy.count(), 1);
        QVERIFY(m_instance->clickToken() == "clickToken");

        QSignalSpy automaticSpy(m_instance, SIGNAL(automaticChanged()));
        m_instance->setAutomatic(!m_instance->automatic());
        QCOMPARE(automaticSpy.count(), 1);
    }

    void testObtainClickToken()
    {
        QSignalSpy clickTokenSpy(m_instance, SIGNAL(clickTokenChanged()));

        std::vector<const UpdatePlugin::ClickUpdateMetadata*> metas;
        QObject::connect(
            m_instance, &UpdatePlugin::ClickUpdateMetadata::clickTokenRequestSucceeded,
            [&](const UpdatePlugin::ClickUpdateMetadata *value) {
                metas.emplace_back(value);
            }
        );

        m_instance->setDownloadUrl("http://localhost:9009/download");
        m_instance->requestClickToken();
        QVERIFY(clickTokenSpy.wait());
        QVERIFY(m_instance->clickToken() == "Mock-X-Click-Token");

        // Make sure we got a meta from the clickTokenRequestSucceeded signal,
        // and that the instance we got is identical to ours.
        QVERIFY(metas.size() == 1);
        QVERIFY(metas.at(0) == m_instance);
    }

    void testEmptyDownloadUrl()
    {
        std::vector<const UpdatePlugin::ClickUpdateMetadata*> metas;
        QObject::connect(
            m_instance, &UpdatePlugin::ClickUpdateMetadata::clickTokenRequestFailed,
            [&](const UpdatePlugin::ClickUpdateMetadata *value) {
                metas.emplace_back(value);
            }
        );
        m_instance->requestClickToken();

        // Make sure we got a meta from the clickTokenRequestSucceeded signal,
        // and that the instance we got is identical to ours.
        QVERIFY(metas.size() == 1);
        QVERIFY(metas.at(0) == m_instance);
    }

private:
    UpdatePlugin::ClickUpdateMetadata *m_instance;
};

QTEST_MAIN(TstClickUpdateMetadata)
#include "tst_clickupdatemetadata.moc"

