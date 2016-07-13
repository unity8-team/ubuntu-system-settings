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

#include <QDate>
#include <QSignalSpy>
#include <QSqlQuery>
#include <QTime>
#include <QTimeZone>
#include <QTest>

#include "update.h"
#include "updatemodel.h"

using namespace UpdatePlugin;

Q_DECLARE_METATYPE(Update::Kinds)
Q_DECLARE_METATYPE(QList<QSharedPointer<Update> >)

class TstUpdateModel : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_model = new UpdateModel(":memory:");
        m_db = m_model->db();
        m_filter = new UpdateModelFilter(m_model, m_model);
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_model, SIGNAL(destroyed(QObject*)));
        m_model->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    QSharedPointer<Update> createUpdate()
    {
        return QSharedPointer<Update>(new Update);
    }
    QSharedPointer<Update> createUpdate(QString id, int rev)
    {
        QSharedPointer<Update> u = createUpdate();
        u->setIdentifier(id);
        u->setRevision(rev);
        return u;
    }
    QSharedPointer<Update> createClickUpdate(QString id, int rev)
    {
        QSharedPointer<Update> u = createUpdate(id, rev);
        u->setKind(Update::Kind::KindClick);
        return u;
    }
    QSharedPointer<Update> createImageUpdate(QString id, int rev)
    {
        QSharedPointer<Update> u = createUpdate(id, rev);
        u->setKind(Update::Kind::KindImage);
        return u;
    }
    void testNoUpdates()
    {
        QCOMPARE(m_model->rowCount(), 0);
    }
    void testAdd()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);

        QSignalSpy insertedSpy(m_model, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)));
        m_db->add(m);
        QTRY_COMPARE(insertedSpy.count(), 1);
        QCOMPARE(m_model->rowCount(), 1);
    }
    void testAddMultiple()
    {
        QSharedPointer<Update> a = createUpdate();
        a->setIdentifier("a.app");
        a->setRevision(1);
        QSharedPointer<Update> b = createUpdate();
        b->setIdentifier("b.app");
        b->setRevision(1);
        QSharedPointer<Update> c = createUpdate();
        c->setIdentifier("c.app");
        c->setRevision(1);

        m_db->add(a);
        m_db->add(b);
        m_db->add(c);

        QCOMPARE(m_model->rowCount(), 3);
    }
    void testRemove()
    {
        QSharedPointer<Update> a = createUpdate();
        a->setIdentifier("a.app");
        a->setRevision(1);

        m_db->add(a);
        QCOMPARE(m_model->rowCount(), 1);

        QSignalSpy removeSpy(m_model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)));
        m_db->remove(a);
        QTRY_COMPARE(removeSpy.count(), 1);
        QCOMPARE(m_model->rowCount(), 0);
    }
    // void testMoveUp()
    // {
    //     /* For moving, we need a filter that sorts. Pending updates are sorted
    //     by title ASC. (See UpdateDb::GET_PENDING) */
    //     m_model->setFilter(UpdateModel::Filter::Pending);

    //     QSharedPointer<Update> a = createUpdate();
    //     a->setIdentifier("first.app");
    //     a->setRevision(1);
    //     a->setTitle("ABC");

    //     QSharedPointer<Update> b = createUpdate();
    //     b->setIdentifier("second.app");
    //     b->setRevision(1);
    //     b->setTitle("CED");

    //     m_db->add(a);
    //     m_db->add(b);

    //     QCOMPARE(m_model->data(m_model->index(0, 0), UpdateModel::Roles::IdRole).toString(),
    //              a->identifier());
    //     QCOMPARE(m_model->data(m_model->index(1, 0), UpdateModel::Roles::IdRole).toString(),
    //              b->identifier());

    //     QSqlQuery q(m_db->db());
    //     q.prepare("UPDATE updates SET title=:title WHERE id=:id AND revision=:revision");
    //     q.bindValue(":title", "XYZ");
    //     q.bindValue(":id", a->identifier());
    //     q.bindValue(":revision", a->revision());
    //     q.exec();
    //     q.finish();

    //     QSignalSpy moveSpy(
    //         m_model, SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int))
    //     );
    //     m_model->refresh();

    //     // Moved and reversed.
    //     QCOMPARE(m_model->data(m_model->index(1, 0), UpdateModel::Roles::IdRole).toString(),
    //              a->identifier());
    //     QCOMPARE(m_model->data(m_model->index(0, 0), UpdateModel::Roles::IdRole).toString(),
    //              b->identifier());

    //     QTRY_COMPARE(moveSpy.count(), 1);

    //     QList<QVariant> args = moveSpy.takeFirst();
    //     // sourceStart == sourceEnd, but destinationRow is 0
    //     QCOMPARE(args.at(1).toInt(), 1);
    //     QCOMPARE(args.at(2).toInt(), 1);
    //     QCOMPARE(args.at(4).toInt(), 0);
    // }
    void testChange()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m->setTitle("old");
        m_db->add(m);

        m->setTitle("updated");
        QSignalSpy dataChangedSpy(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
        m_db->add(m);
        QTRY_COMPARE(dataChangedSpy.count(), 1);
        QList<QVariant> args = dataChangedSpy.takeFirst();
        QCOMPARE(args.at(0).toInt(), 0);
    }
    void testMultipleChanges()
    {
        QSharedPointer<Update> a = createUpdate();
        a->setIdentifier("a.app");
        a->setRevision(1);
        a->setTitle("a");

        QSharedPointer<Update> b = createUpdate();
        b->setIdentifier("b.app");
        b->setRevision(1);
        b->setTitle("b");

        QSharedPointer<Update> c = createUpdate();
        c->setIdentifier("c.app");
        c->setRevision(1);
        c->setTitle("c");

        m_db->add(a);
        m_db->add(b);
        m_db->add(c);

        QVector<QSharedPointer<Update>> list;
        list << a << b << c;

        // Change three titles.
        Q_FOREACH(const QSharedPointer<Update> u, list) {
            QSqlQuery q(m_db->db());
            q.prepare("UPDATE updates SET title=:title WHERE id=:id AND revision=:revision");
            q.bindValue(":title", u->title() + "-new");
            q.bindValue(":id", u->identifier());
            q.bindValue(":revision", u->revision());
            q.exec();
            q.finish();
        }

        QSignalSpy dataChangedSpy(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
        m_model->refresh();
        QTRY_COMPARE(dataChangedSpy.count(), 3);
        QList<QVariant> args = dataChangedSpy.takeFirst();
    }
    // // TODO: move this test to updatedb.
    // void testSupersededUpdate()
    // {
    //     QSharedPointer<Update> superseded = createUpdate();
    //     superseded->setIdentifier("some.app");
    //     superseded->setRevision(1);
    //     superseded->setKind(Update::Kind::KindClick);

    //     QSharedPointer<Update> replacement = createUpdate();
    //     replacement->setIdentifier("some.app");
    //     replacement->setRevision(2);
    //     replacement->setKind(Update::Kind::KindClick);

    //     m_db->add(superseded);
    //     m_db->add(replacement);

    //     // We only want the replacement in our model of pending updates.
    //     m_model->setFilter(UpdateModel::Filter::PendingClicks);
    //     QCOMPARE(m_model->rowCount(), 1);
    //     QCOMPARE(m_model->data(
    //         m_model->index(0), UpdateModel::IdRole
    //     ).toString(), replacement->identifier());
    // }
    void testRoles()
    {
        QSharedPointer<Update> app = createUpdate();
        QStringList mc;
        mc << "ls" << "la";

        app->setKind(Update::Kind::KindClick);
        app->setIdentifier("com.ubuntu.testapp");
        app->setLocalVersion("0.1");
        app->setRemoteVersion("0.2");
        app->setRevision(42);
        app->setTitle("Test App");
        app->setDownloadHash("987654323456789");
        app->setBinaryFilesize(1000);
        app->setIconUrl("http://example.org/testapp.png");
        app->setDownloadUrl("http://example.org/testapp.click");
        app->setCommand(mc);
        app->setChangelog("* Fixed all bugs * Introduced new bugs");
        app->setToken("token");
        app->setProgress(50);
        app->setState(Update::State::StateInstallPaused);
        app->setError("Failure");
        app->setPackageName("testapp");

        m_db->add(app);
        m_model->refresh();

        QModelIndex idx = m_model->index(0);

        QCOMPARE(m_model->data(idx, UpdateModel::KindRole).toUInt(), (uint) Update::Kind::KindClick);
        QCOMPARE(m_model->data(idx, UpdateModel::IdRole).toString(), app->identifier());
        QCOMPARE(m_model->data(idx, UpdateModel::LocalVersionRole).toString(), app->localVersion());
        QCOMPARE(m_model->data(idx, UpdateModel::RemoteVersionRole).toString(), app->remoteVersion());
        QCOMPARE(m_model->data(idx, UpdateModel::RevisionRole).toUInt(), app->revision());
        QCOMPARE(m_model->data(idx, UpdateModel::TitleRole).toString(), app->title());
        QCOMPARE(m_model->data(idx, UpdateModel::DownloadHashRole).toString(), app->downloadHash());
        QCOMPARE(m_model->data(idx, UpdateModel::SizeRole).toUInt(), app->binaryFilesize());
        QCOMPARE(m_model->data(idx, UpdateModel::IconUrlRole).toString(), app->iconUrl());
        QCOMPARE(m_model->data(idx, UpdateModel::DownloadUrlRole).toString(), app->downloadUrl());
        QCOMPARE(m_model->data(idx, UpdateModel::CommandRole).toStringList(), app->command());
        QCOMPARE(m_model->data(idx, UpdateModel::ChangelogRole).toString(), app->changelog());
        QCOMPARE(m_model->data(idx, UpdateModel::TokenRole).toString(), app->token());
        QCOMPARE(m_model->data(idx, UpdateModel::InstalledRole).toBool(), false);
        QCOMPARE(m_model->data(idx, UpdateModel::AutomaticRole).toBool(), app->automatic());
        QCOMPARE(m_model->data(idx, UpdateModel::UpdateStateRole).toUInt(), (uint) Update::State::StateInstallPaused);
        QCOMPARE(m_model->data(idx, UpdateModel::ProgressRole).toInt(), 50);
        QCOMPARE(m_model->data(idx, UpdateModel::ErrorRole).toString(), QString("Failure"));
        QCOMPARE(m_model->data(idx, UpdateModel::PackageNameRole).toString(), QString("testapp"));

        // Verify that the date ain't empty.
        QVERIFY(m_model->data(idx, UpdateModel::CreatedAtRole).toDateTime().isValid());
        QVERIFY(!m_model->data(idx, UpdateModel::UpdatedAtRole).toDateTime().isValid());

    }
    void testNotFound()
    {
        QSharedPointer<Update> notFound = m_model->get("notfound", 0);
        QVERIFY(notFound.isNull());
    }
    void testImageUpdate()
    {
        m_model->setImageUpdate("ubuntu", "350", 400000);
        QCOMPARE(m_model->rowCount(), 1);
        QCOMPARE(m_model->data(
            m_model->index(0), UpdateModel::IdRole
        ).toString(), QString("ubuntu"));
        QCOMPARE(m_model->data(
            m_model->index(0), UpdateModel::RevisionRole
        ).toUInt(), (uint) 350);
        QCOMPARE(m_model->data(
            m_model->index(0), UpdateModel::AutomaticRole
        ).toBool(), false);

    }
    void testSetAvailable()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        u->setError("Some error");
        u->setProgress(95);
        u->setToken("foobar");
        m_db->add(u);
        m_model->setAvailable(u->identifier(), u->revision());
        QSharedPointer<Update> u1 = m_model->get(u->identifier(), u->revision());

        QVERIFY(!u1.isNull());

        QCOMPARE(u1->error(), QString(""));
        QCOMPARE(u1->progress(), 0);
        QCOMPARE(u1->token(), QString(""));
        QCOMPARE(u1->state(), Update::State::StateAvailable);
    }
    void testSetInstalled()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->setInstalled(u->identifier(), u->revision());
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateInstallFinished);
    }
    void testSetError()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->setError(u->identifier(), u->revision(), "fail");
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateFailed);
    }
    void testSetProgress()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->setProgress(u->identifier(), u->revision(), 5);
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateDownloading);
    }
    void testSetDownloaded()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->setDownloaded(u->identifier(), u->revision());
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateDownloaded);
    }
    void testStartUpdate()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->startUpdate(u->identifier(), u->revision());
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateDownloading);
    }
    void testQueueUpdate()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->queueUpdate(u->identifier(), u->revision());
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateQueuedForDownload);
    }
    void testProcessUpdate()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->processUpdate(u->identifier(), u->revision());
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateInstalling);
    }
    void testPauseUpdate()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->pauseUpdate(u->identifier(), u->revision());
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateDownloadPaused);
    }
    void testResumeUpdate()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->resumeUpdate(u->identifier(), u->revision());
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateDownloading);
    }
    void testCancelUpdate()
    {
        QSharedPointer<Update> u = createUpdate("id", 42);
        m_db->add(u);
        m_model->cancelUpdate(u->identifier(), u->revision());
        QCOMPARE(m_db->get(u->identifier(), u->revision())->state(),
                 Update::State::StateAvailable);
    }
    void testRoleNames()
    {
        QHash<int, QByteArray> names = m_model->roleNames();
        QVERIFY(names[Qt::DisplayRole] == "displayName");
        QVERIFY(names[UpdateModel::Roles::KindRole] == "kind");
        QVERIFY(names[UpdateModel::Roles::IconUrlRole] == "iconUrl");
        QVERIFY(names[UpdateModel::Roles::IdRole] == "identifier");
        QVERIFY(names[UpdateModel::Roles::LocalVersionRole] == "localVersion");
        QVERIFY(names[UpdateModel::Roles::RemoteVersionRole] == "remoteVersion");
        QVERIFY(names[UpdateModel::Roles::RevisionRole] == "revision");
        QVERIFY(names[UpdateModel::Roles::InstalledRole] == "installed");
        QVERIFY(names[UpdateModel::Roles::CreatedAtRole] == "createdAt");
        QVERIFY(names[UpdateModel::Roles::UpdatedAtRole] == "updatedAt");
        QVERIFY(names[UpdateModel::Roles::TitleRole] == "title");
        QVERIFY(names[UpdateModel::Roles::DownloadHashRole] == "downloadHash");
        QVERIFY(names[UpdateModel::Roles::SizeRole] == "size");
        QVERIFY(names[UpdateModel::Roles::DownloadUrlRole] == "downloadUrl");
        QVERIFY(names[UpdateModel::Roles::ChangelogRole] == "changelog");
        QVERIFY(names[UpdateModel::Roles::CommandRole] == "command");
        QVERIFY(names[UpdateModel::Roles::TokenRole] == "token");
        QVERIFY(names[UpdateModel::Roles::UpdateStateRole] == "updateState");
        QVERIFY(names[UpdateModel::Roles::ProgressRole] == "progress");
        QVERIFY(names[UpdateModel::Roles::AutomaticRole] == "automatic");
        QVERIFY(names[UpdateModel::Roles::ErrorRole] == "error");
        QVERIFY(names[UpdateModel::Roles::PackageNameRole] == "packageName");
    }
    void testFilterKinds_data()
    {
        QTest::addColumn<Update::Kinds>("kinds");

        Update::Kinds unknown(Update::Kind::KindUnknown);
        QTest::newRow("Unknown") << unknown;

        Update::Kinds clickAndImages(Update::Kind::KindImage | Update::Kind::KindClick);
        QTest::newRow("Click and Images") << clickAndImages;

        Update::Kinds clicks(Update::Kind::KindClick);
        QTest::newRow("Click") << clicks;

        Update::Kinds images(Update::Kind::KindImage);
        QTest::newRow("Images") << images;
    }
    void testFilterKinds()
    {
        QFETCH(Update::Kinds, kinds);

        m_filter->filterOnKinds(kinds);
        QCOMPARE(m_filter->kinds(), kinds);
    }
    void testFilterInstalled_data()
    {
        QTest::addColumn<bool>("installed");

        QTest::newRow("Installed") << true;
        QTest::newRow("Not installed") << false;
    }
    void testFilterInstalled()
    {
        QFETCH(bool, installed);

        m_filter->filterOnInstalled(installed);
        QCOMPARE(m_filter->installed(), installed);
    }
    void testFilterKindsIntegration_data()
    {
        QTest::addColumn<QList<QSharedPointer<Update>> >("updates");
        QTest::addColumn<Update::Kinds>("kindsFilter");
        QTest::addColumn<int>("targetCount");

        QList<QSharedPointer<Update>> sample;
        sample << createClickUpdate("a", 1) << createClickUpdate("b", 2)
               << createImageUpdate("u", 1);

        Update::Kinds clicks(Update::Kind::KindClick);
        Update::Kinds images(Update::Kind::KindImage);

        QTest::newRow("Filter on clicks") << sample << clicks << 2;
        QTest::newRow("Filter on images") << sample << images << 1;

    }
    void testFilterKindsIntegration()
    {
        QFETCH(QList<QSharedPointer<Update> >, updates);
        QFETCH(Update::Kinds, kindsFilter);
        QFETCH(int, targetCount);

        Q_FOREACH(QSharedPointer<Update> u, updates) {
            m_model->add(u);
        }

        m_filter->filterOnKinds(kindsFilter);
        QCOMPARE(m_filter->rowCount(), targetCount);
    }
    void testFilteringInstalledIntegration_data()
    {
        QTest::addColumn<QList<QSharedPointer<Update>> >("updates");
        QTest::addColumn<bool>("installedFilter");
        QTest::addColumn<int>("targetCount");


        QSharedPointer<Update> installed = createUpdate("a", 1);
        installed->setInstalled(true);

        QList<QSharedPointer<Update>> sample;
        sample << createUpdate("b", 1) << createUpdate("c", 1) << installed;

        QTest::newRow("Filter not installed") << sample << false << 2;
        QTest::newRow("Filter installed") << sample << true << 1;

    }
    void testFilteringInstalledIntegration()
    {
        QFETCH(QList<QSharedPointer<Update> >, updates);
        QFETCH(bool, installedFilter);
        QFETCH(int, targetCount);

        Q_FOREACH(QSharedPointer<Update> u, updates) {
            m_model->add(u);
        }

        m_filter->filterOnInstalled(installedFilter);
        QCOMPARE(m_filter->rowCount(), targetCount);
    }
private:
    UpdateDb *m_db = nullptr;
    UpdateModel *m_model = nullptr;
    UpdateModelFilter *m_filter = nullptr;
};

QTEST_MAIN(TstUpdateModel)

#include "tst_updatemodel.moc"
