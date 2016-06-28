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

#include <QSignalSpy>
#include <QTest>

#include "update.h"

using namespace UpdatePlugin;

Q_DECLARE_METATYPE(Update::Kind)

class TstUpdate
    : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_instance = new Update();
    }
    void cleanup()
    {
        m_instance->deleteLater();
    }
    void cleanupTestCase()
    {
        /* Allow for deleteLater of QObjects in this test to be deleted.
        Workaround for QTBUG-12575. */
        QTest::qWait(200);
    }
    void testProperties()
    {
        QStringList depts; depts << "foo" << "bar";

        QSignalSpy kindSpy(m_instance, SIGNAL(kindChanged()));
        m_instance->setKind(Update::Kind::KindClick);
        QCOMPARE(kindSpy.count(), 1);
        QCOMPARE(m_instance->kind(), Update::Kind::KindClick);

        QSignalSpy identifierSpy(m_instance, SIGNAL(identifierChanged()));
        m_instance->setIdentifier("identifier");
        QCOMPARE(identifierSpy.count(), 1);
        QCOMPARE(m_instance->identifier(), QString("identifier"));

        QSignalSpy anonDownloadUrlSpy(m_instance, SIGNAL(anonDownloadUrlChanged()));
        m_instance->setAnonDownloadUrl("anonDownloadUrl");
        QCOMPARE(anonDownloadUrlSpy.count(), 1);
        QCOMPARE(m_instance->anonDownloadUrl(), QString("anonDownloadUrl"));

        QSignalSpy binaryFilesizeSpy(m_instance, SIGNAL(binaryFilesizeChanged()));
        m_instance->setBinaryFilesize(1000);
        QCOMPARE(binaryFilesizeSpy.count(), 1);
        QCOMPARE(m_instance->binaryFilesize(), (uint) 1000);

        QSignalSpy changelogSpy(m_instance, SIGNAL(changelogChanged()));
        m_instance->setChangelog("changelog");
        QCOMPARE(changelogSpy.count(), 1);
        QCOMPARE(m_instance->changelog(), QString("changelog"));

        QSignalSpy channelSpy(m_instance, SIGNAL(channelChanged()));
        m_instance->setChannel("channel");
        QCOMPARE(channelSpy.count(), 1);
        QCOMPARE(m_instance->channel(), QString("channel"));

        QSignalSpy contentSpy(m_instance, SIGNAL(contentChanged()));
        m_instance->setContent("content");
        QCOMPARE(contentSpy.count(), 1);
        QCOMPARE(m_instance->content(), QString("content"));

        QSignalSpy departmentSpy(m_instance, SIGNAL(departmentChanged()));
        m_instance->setDepartment(depts);
        QCOMPARE(departmentSpy.count(), 1);
        QCOMPARE(m_instance->department(), depts);

        QSignalSpy downloadIdSpy(m_instance, SIGNAL(downloadIdChanged()));
        m_instance->setDownloadId("downloadId");
        QCOMPARE(downloadIdSpy.count(), 1);
        QCOMPARE(m_instance->downloadId(), QString("downloadId"));

        QSignalSpy downloadHash(m_instance, SIGNAL(downloadHashChanged()));
        m_instance->setDownloadHash("downloadHash");
        QCOMPARE(downloadHash.count(), 1);
        QCOMPARE(m_instance->downloadHash(), QString("downloadHash"));

        QSignalSpy downloadUrlSpy(m_instance, SIGNAL(downloadUrlChanged()));
        m_instance->setDownloadUrl("downloadUrl");
        QCOMPARE(downloadUrlSpy.count(), 1);
        QCOMPARE(m_instance->downloadUrl(), QString("downloadUrl"));

        QSignalSpy iconUrlSpy(m_instance, SIGNAL(iconUrlChanged()));
        m_instance->setIconUrl("iconUrl");
        QCOMPARE(iconUrlSpy.count(), 1);
        QCOMPARE(m_instance->iconUrl(), QString("iconUrl"));

        QSignalSpy installedSpy(m_instance, SIGNAL(installedChanged()));
        m_instance->setInstalled(true);
        QCOMPARE(installedSpy.count(), 1);
        QCOMPARE(m_instance->installed(), true);

        QSignalSpy originSpy(m_instance, SIGNAL(originChanged()));
        m_instance->setOrigin("origin");
        QCOMPARE(originSpy.count(), 1);
        QCOMPARE(m_instance->origin(), QString("origin"));

        QDateTime createdAt(QDate(2016, 2, 29), QTime(18, 0), Qt::UTC);
        QSignalSpy createdAtSpy(m_instance, SIGNAL(createdAtChanged()));
        m_instance->setCreatedAt(createdAt);
        QCOMPARE(createdAtSpy.count(), 1);
        QCOMPARE(m_instance->createdAt(), createdAt);

        QDateTime updatedAt(QDate(2016, 2, 29), QTime(18, 0), Qt::UTC);
        QSignalSpy updatedAtSpy(m_instance, SIGNAL(updatedAtChanged()));
        m_instance->setUpdatedAt(updatedAt);
        QCOMPARE(updatedAtSpy.count(), 1);
        QCOMPARE(m_instance->updatedAt(), updatedAt);

        QSignalSpy revisionSpy(m_instance, SIGNAL(revisionChanged()));
        m_instance->setRevision(10);
        QCOMPARE(revisionSpy.count(), 1);
        QCOMPARE(m_instance->revision(), 10);

        QSignalSpy stateSpy(m_instance, SIGNAL(stateChanged()));
        m_instance->setState(Update::State::StateFailed);
        QCOMPARE(stateSpy.count(), 1);
        QCOMPARE(m_instance->state(), Update::State::StateFailed);

        QSignalSpy titleSpy(m_instance, SIGNAL(titleChanged()));
        m_instance->setTitle("title");
        QCOMPARE(titleSpy.count(), 1);
        QCOMPARE(m_instance->title(), QString("title"));

        QSignalSpy remoteVersionSpy(m_instance, SIGNAL(remoteVersionChanged()));
        m_instance->setRemoteVersion("version");
        QCOMPARE(remoteVersionSpy.count(), 1);
        QCOMPARE(m_instance->remoteVersion(), QString("version"));

        QSignalSpy localVersionSpy(m_instance, SIGNAL(localVersionChanged()));
        m_instance->setLocalVersion("version");
        QCOMPARE(localVersionSpy.count(), 1);
        QCOMPARE(m_instance->localVersion(), QString("version"));

        QSignalSpy tokenSpy(m_instance, SIGNAL(tokenChanged()));
        m_instance->setToken("token");
        QCOMPARE(tokenSpy.count(), 1);
        QCOMPARE(m_instance->token(), QString("token"));

        QStringList command;
        command << "ls" << "-la";
        QSignalSpy commandSpy(m_instance, SIGNAL(commandChanged()));
        m_instance->setCommand(command);
        QCOMPARE(commandSpy.count(), 1);
        QCOMPARE(m_instance->command(), command);

        QSignalSpy automaticSpy(m_instance, SIGNAL(automaticChanged()));
        m_instance->setAutomatic(!m_instance->automatic());
        QCOMPARE(automaticSpy.count(), 1);
    }
    void testUpdateRequired()
    {
        m_instance->setLocalVersion("1.0.0");
        m_instance->setRemoteVersion("1.0.1");
        QCOMPARE(m_instance->isUpdateRequired(), true);

        m_instance->setLocalVersion("1.0.1");
        m_instance->setRemoteVersion("1.0.1");
        QCOMPARE(m_instance->isUpdateRequired(), false);
    }
    void testDeepEquals_data()
    {
        QTest::addColumn<Update*>("a");
        QTest::addColumn<Update*>("b");
        QTest::addColumn<bool>("equals");

        QDateTime created_a(QDate(2016, 2, 29));
        QDateTime updated_a(QDate(2016, 2, 28));

        QDateTime created_b(QDate(2016, 2, 21));
        QDateTime updated_b(QDate(2016, 2, 22));

        Update* a = new Update;
        a->setKind(Update::Kind::KindClick);
        a->setIdentifier("id_a");
        a->setLocalVersion("localVersion_a");
        a->setRemoteVersion("remoteVersion_a");
        a->setRevision(1);
        a->setInstalled(true);
        a->setCreatedAt(created_a);
        a->setUpdatedAt(updated_a);
        a->setTitle("title_a");
        a->setDownloadHash("downloadHash_a");
        a->setBinaryFilesize(1000);
        a->setIconUrl("iconUrl_a");
        a->setDownloadUrl("downloadUrl_a");
        a->setCommand(QStringList());
        a->setChangelog("changelog_a");
        a->setToken("token_a");
        a->setState(Update::State::StateAvailable);
        a->setProgress(0);
        a->setAutomatic(false);
        a->setDownloadId("downloadId_a");
        a->setError("error_a");

        Update* b = new Update;
        b->setKind(Update::Kind::KindImage);
        b->setIdentifier("id_b");
        b->setLocalVersion("localVersion_b");
        b->setRemoteVersion("remoteVersion_b");
        b->setRevision(1);
        b->setInstalled(false);
        b->setCreatedAt(created_b);
        b->setUpdatedAt(updated_b);
        b->setTitle("title_b");
        b->setDownloadHash("downloadHash_b");
        b->setBinaryFilesize(1000);
        b->setIconUrl("iconUrl_b");
        b->setDownloadUrl("downloadUrl_b");
        b->setCommand(QStringList());
        b->setChangelog("changelog_b");
        b->setToken("token_b");
        b->setState(Update::State::StateFailed);
        b->setProgress(0);
        b->setAutomatic(false);
        b->setDownloadId("downloadId_b");
        b->setError("error_b");

        QTest::newRow("Equality") << a << a << true;
        QTest::newRow("Inequality") << a << b << false;
    }
    void testDeepEquals()
    {
        QFETCH(Update*, a);
        QFETCH(Update*, b);
        QFETCH(bool, equals);

        QCOMPARE(a->deepEquals(b), equals);
        QCOMPARE(b->deepEquals(a), equals);

        a->deleteLater();
        b->deleteLater();
    }
    void testShallowEquality_data()
    {
        QTest::addColumn<Update*>("a");
        QTest::addColumn<Update*>("b");
        QTest::addColumn<bool>("equals");

        Update* a = new Update;
        a->setIdentifier("id_a");
        a->setRevision(1);

        Update* a1 = new Update;
        a1->setDownloadId("abc");

        Update* b = new Update;
        b->setIdentifier("id_b");
        b->setRevision(2);

        Update* b1 = new Update;
        b1->setDownloadId("xyz");

        QTest::newRow("Id/rev equality") << a << a << true;
        QTest::newRow("Download id equality") << a1 << a1 << true;

        QTest::newRow("Id/rev inequality") << a << b << false;
        QTest::newRow("Download id inequality") << a1 << b1 << false;
    }
    void testShallowEquality()
    {
        QFETCH(Update*, a);
        QFETCH(Update*, b);
        QFETCH(bool, equals);

        QCOMPARE(a == b, equals);
        QCOMPARE(b == a, equals);

        a->deleteLater();
        b->deleteLater();
    }
private:
    Update *m_instance;
};

QTEST_MAIN(TstUpdate)
#include "tst_update.moc"

