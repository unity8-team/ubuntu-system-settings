#include <QDate>
#include <QFile>
#include <QSignalSpy>
#include <QSqlRecord>
#include <QTemporaryDir>
#include <QTime>
#include <QTimeZone>
#include <QTest>

#include "mockclickservertestcase.h"

#include "clickupdatemetadata.h"
#include "clickupdatestore.h"

class TstClickUpdateStore
    : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
    }
    void cleanupTestCase()
    {
    }
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_dbfile = m_dir->path() + "/cupdatesstore.db";
        qWarning() << "test using dbfile"  << m_dbfile;
        m_instance = new UpdatePlugin::ClickUpdateStore(m_dbfile);
    }
    void cleanup()
    {
        delete m_instance;
        delete m_dir;
    }
    void testLastCheck()
    {
        QCOMPARE(m_instance->lastCheckDate().isValid(), false);

        // 29th of February 2016 at 20:00 UTC+2
        QDateTime d(QDate(2016, 2, 29), QTime(20,0), QTimeZone(2 * 3600));

        // Same date, in UTC.
        QDateTime dUtc(QDate(2016, 2, 29), QTime(18,0), Qt::UTC);

        m_instance->setLastCheckDate(d);

        QCOMPARE(m_instance->lastCheckDate(), dUtc);
    }
    void testUpdateLifecycle()
    {
        UpdatePlugin::ClickUpdateMetadata m;
        m.setRevision(42);
        m.setName("com.ubuntu.testapp");
        m.setLocalVersion("0.1");
        m.setRemoteVersion("0.2");
        m.setTitle("Test App");
        m.setBinaryFilesize(1000);
        m.setIconUrl("http://example.org/testapp.png");
        m.setDownloadUrl("http://example.org/testapp.click");
        m.setChangelog("* Fixed all bugs * Introduced new bugs");
        m.setClickToken("Mock-X-Click-Token");
        m.setDownloadSha512("987654323456789");

        UpdatePlugin::ClickUpdateMetadata m2;
        m2.setRevision(100);
        m2.setName("com.ubuntu.myapp");
        m2.setLocalVersion("1.1");
        m2.setRemoteVersion("1.2");
        m2.setTitle("My app");
        m2.setBinaryFilesize(2000);
        m2.setIconUrl("http://example.org/myapp.png");
        m2.setDownloadUrl("http://example.org/myapp.click");
        m2.setChangelog("* First version");
        m2.setClickToken("Mock-X-Click-Token");
        m2.setDownloadSha512("293847");

        // Add a click app
        m_instance->add(&m);
        QCOMPARE(m_instance->activeUpdates()->rowCount(), 1);
        QCOMPARE(m_instance->installedUpdates()->rowCount(), 0);

        // We had to refresh tokens.
        m.setClickToken("New-Mock-X-Click-Token");
        m_instance->add(&m);
        QCOMPARE(m_instance->activeUpdates()->rowCount(), 1);
        QCOMPARE(m_instance->installedUpdates()->rowCount(), 0);

        // Associate with udm
        m_instance->setUdmId(m.name(), m.revision(), 3);
        QCOMPARE(m_instance->activeUpdates()->record(0).value("udm_download_id").toInt(),
                 3);

        // Disassociate with udm
        m_instance->unsetUdmId(m.name(), m.revision());
        QVERIFY(m_instance->activeUpdates()
                    ->record(0).value("udm_download_id").isNull());

        // Add second click app
        m_instance->add(&m2);
        QCOMPARE(m_instance->activeUpdates()->rowCount(), 2);
        QCOMPARE(m_instance->installedUpdates()->rowCount(), 0);

        QCOMPARE(m_instance->activeUpdates()
                    ->record(0).value("app_id").toString(),
                 m2.name());
        QCOMPARE(m_instance->activeUpdates()
                    ->record(1).value("app_id").toString(),
                 m.name());

        // Pruning here should have no effect.
        m_instance->pruneDb();

        // Mark as installed
        m_instance->markInstalled(m.name(), m.revision());
        QCOMPARE(m_instance->activeUpdates()->rowCount(), 1);
        QCOMPARE(m_instance->installedUpdates()->rowCount(), 1);
    }

private:
    UpdatePlugin::ClickUpdateStore *m_instance;
    QTemporaryDir *m_dir;
    QString m_dbfile;
};

QTEST_MAIN(TstClickUpdateStore)
#include "tst_clickupdatestore.moc"
