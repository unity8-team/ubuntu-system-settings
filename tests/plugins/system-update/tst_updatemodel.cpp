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

Q_DECLARE_METATYPE(QList<QSharedPointer<Update> >)
Q_DECLARE_METATYPE(QSharedPointer<Update>)
Q_DECLARE_METATYPE(Update::Kind)
Q_DECLARE_METATYPE(UpdateModel::Roles)

class TstUpdateModel : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_model = new UpdateModel(":memory:");
        m_db = m_model->db();
        m_filter = new UpdateModelFilter(m_model);
        m_filter->setSourceModel(m_model);
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
        auto update = createUpdate();
        update->setIdentifier(id);
        update->setRevision(rev);
        return update;
    }
    QSharedPointer<Update> createClickUpdate(QString id, int rev)
    {
        auto update = createUpdate(id, rev);
        update->setKind(Update::Kind::KindClick);
        return update;
    }
    QSharedPointer<Update> createImageUpdate(QString id, int rev)
    {
        auto update = createUpdate(id, rev);
        update->setKind(Update::Kind::KindImage);
        return update;
    }
    void testNoUpdates()
    {
        QCOMPARE(m_model->rowCount(), 0);
    }
    void testAdd()
    {
        auto update = createUpdate();
        update->setIdentifier("test.app");
        update->setRevision(1);

        QSignalSpy insertedSpy(m_model, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)));
        m_db->add(update);
        QTRY_COMPARE(insertedSpy.count(), 1);
        QCOMPARE(m_model->rowCount(), 1);
    }
    void testAddMultiple()
    {
        auto a = createUpdate();
        a->setIdentifier("a.app");
        a->setRevision(1);
        auto b = createUpdate();
        b->setIdentifier("b.app");
        b->setRevision(1);
        auto c = createUpdate();
        c->setIdentifier("c.app");
        c->setRevision(1);

        m_db->add(a);
        m_db->add(b);
        m_db->add(c);

        QCOMPARE(m_model->rowCount(), 3);
    }
    void testRemove()
    {
        auto update = createUpdate();
        update->setIdentifier("a.app");
        update->setRevision(1);

        m_db->add(update);
        QSignalSpy removeSpy(m_model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)));
        m_model->remove(update);
        QTRY_COMPARE(removeSpy.count(), 1);
        QCOMPARE(m_model->rowCount(), 0);

        m_db->add(update);
        m_model->remove(update->identifier(), update->revision());
        QTRY_COMPARE(removeSpy.count(), 2);
        QCOMPARE(m_model->rowCount(), 0);
    }
    void testChange()
    {
        auto update = createUpdate();
        update->setIdentifier("test.app");
        update->setRevision(1);
        update->setTitle("old");
        m_db->add(update);

        update->setTitle("updated");
        QSignalSpy dataChangedSpy(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
        m_db->add(update);
        QTRY_COMPARE(dataChangedSpy.count(), 1);
        QList<QVariant> args = dataChangedSpy.takeFirst();
        QCOMPARE(args.at(0).toInt(), 0);
    }
    void testMultipleChanges()
    {
        auto a = createUpdate();
        a->setIdentifier("a.app");
        a->setRevision(1);
        a->setTitle("a");

        auto b = createUpdate();
        b->setIdentifier("b.app");
        b->setRevision(1);
        b->setTitle("b");

        auto c = createUpdate();
        c->setIdentifier("c.app");
        c->setRevision(1);
        c->setTitle("c");

        m_db->add(a);
        m_db->add(b);
        m_db->add(c);

        QVector<QSharedPointer<Update>> list;
        list << a << b << c;

        // Change three titles.
        Q_FOREACH(auto update, list) {
            QSqlQuery q(m_db->db());
            q.prepare("UPDATE updates SET title=:title WHERE id=:id AND revision=:revision");
            q.bindValue(":title", update->title() + "-new");
            q.bindValue(":id", update->identifier());
            q.bindValue(":revision", update->revision());
            q.exec();
            q.finish();
        }

        QSignalSpy dataChangedSpy(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
        m_model->refresh();
        QTRY_COMPARE(dataChangedSpy.count(), 3);
        QList<QVariant> args = dataChangedSpy.takeFirst();
    }
    void testRoles_data()
    {
        QTest::addColumn<UpdateModel::Roles>("role");
        QTest::addColumn<QVariant>("value");
        QTest::addColumn<QSharedPointer<Update>>("update");

        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::KindRole);
            QVariant value((uint) Update::Kind::KindClick);
            update->setKind(Update::Kind::KindClick);
            QTest::newRow("KindRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::IdRole);
            QVariant value("test");
            QTest::newRow("IdRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::LocalVersionRole);
            QVariant value("v1");
            update->setLocalVersion("v1");
            QTest::newRow("IdRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::RemoteVersionRole);
            QVariant value("0.2");
            update->setRemoteVersion("0.2");
            QTest::newRow("RemoteVersionRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::RevisionRole);
            QVariant value(42);
            update->setRevision(42);
            QTest::newRow("RevisionRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::TitleRole);
            QVariant value("Test App");
            update->setTitle("Test App");
            QTest::newRow("TitleRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::DownloadHashRole);
            QVariant value("987654323456789");
            update->setDownloadHash("987654323456789");
            QTest::newRow("DownloadHashRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::DownloadIdRole);
            QVariant value("someuuid");
            update->setDownloadId("someuuid");
            QTest::newRow("DownloadIdRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::SizeRole);
            QVariant value(1000);
            update->setBinaryFilesize(1000);
            QTest::newRow("SizeRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::IconUrlRole);
            QVariant value("http://example.org/testapp.png");
            update->setIconUrl("http://example.org/testapp.png");
            QTest::newRow("IconUrlRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::DownloadUrlRole);
            QVariant value("http://example.org/testapp.click");
            update->setDownloadUrl("http://example.org/testapp.click");
            QTest::newRow("DownloadUrlRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::CommandRole);
            QStringList mc;
            mc << "ls" << "-la";
            QVariant value(mc);
            update->setCommand(mc);
            QTest::newRow("CommandRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::ChangelogRole);
            QVariant value("* Fixed all bugs * Introduced new bugs");
            update->setChangelog("* Fixed all bugs * Introduced new bugs");
            QTest::newRow("ChangelogRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::TokenRole);
            QVariant value("token");
            update->setToken("token");
            QTest::newRow("TokenRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::ProgressRole);
            QVariant value(50);
            update->setProgress(50);
            QTest::newRow("ProgressRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::UpdateStateRole);
            QVariant value((uint) Update::State::StateInstallPaused);
            update->setState(Update::State::StateInstallPaused);
            QTest::newRow("UpdateStateRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::ErrorRole);
            QVariant value("Failure");
            update->setError("Failure");
            QTest::newRow("ErrorRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::PackageNameRole);
            QVariant value("testapp");
            update->setPackageName("testapp");
            QTest::newRow("PackageNameRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            UpdateModel::Roles role(UpdateModel::SignedDownloadUrlRole);
            QVariant value("signedUrl");
            update->setSignedDownloadUrl("signedUrl");
            QTest::newRow("SignedDownloadUrlRole") << role << value << update;
        }
        {
            auto update = createUpdate("test", 1);
            auto target = QDateTime::currentDateTimeUtc();
            UpdateModel::Roles role(UpdateModel::UpdatedAtRole);
            QVariant value(target);
            update->setUpdatedAt(target);
            QTest::newRow("UpdatedAtRole") << role << value << update;
        }
    }
    void testRoles()
    {
        QFETCH(UpdateModel::Roles, role);
        QFETCH(QVariant, value);
        QFETCH(QSharedPointer<Update>, update);

        m_model->add(update);
        QCOMPARE(m_model->data(m_model->index(0), role), value);
    }
    void testCreatedAtRole()
    {
        m_model->add(createUpdate("test", 1));
        // Just make sure the createdAt role isn't empty.
        QVERIFY(m_model->data(
            m_model->index(0),
            UpdateModel::CreatedAtRole).toDateTime().isValid()
        );
    }
    void testNotFound()
    {
        auto notFound = m_model->get("notfound", 0);
        QVERIFY(notFound.isNull());
    }
    void testImageUpdate()
    {
        m_model->setImageUpdate("ubuntu", 350, 400000);
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
        auto update = createUpdate("id", 42);
        update->setError("Some error");
        update->setProgress(95);
        update->setToken("foobar");
        m_db->add(update);
        m_model->setAvailable(update->identifier(), update->revision());

        auto update1 = m_model->get(update->identifier(), update->revision());

        QVERIFY(!update1.isNull());

        QCOMPARE(update1->error(), QString(""));
        QCOMPARE(update1->progress(), 0);
        QCOMPARE(update1->token(), QString(""));
        QCOMPARE(update1->downloadId(), QString(""));
        QCOMPARE(update1->state(), Update::State::StateAvailable);
    }
    void testSetInstalled()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->setInstalled(update->identifier(), update->revision());
        auto update1 = m_db->get(update->identifier(), update->revision());
        QCOMPARE(update1->state(), Update::State::StateInstallFinished);
        QCOMPARE(update1->downloadId(), QString(""));
    }
    void testSetError()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->setError(update->identifier(), update->revision(), "fail");

        auto update1 = m_db->get(update->identifier(), update->revision());
        QCOMPARE(update1->state(), Update::State::StateFailed);
        QCOMPARE(update1->downloadId(), QString(""));
    }
    void testSetProgress()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->setProgress(update->identifier(), update->revision(), 5);
        auto update1 = m_db->get(update->identifier(), update->revision());
        QCOMPARE(update1->state(), Update::State::StateDownloading);
        QCOMPARE(update1->progress(), 5);
    }
    void testSetInstalling()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->setInstalling(update->identifier(), update->revision(), 5);
        auto update1 = m_db->get(update->identifier(), update->revision());
        QCOMPARE(update1->state(), Update::State::StateInstalling);
        QCOMPARE(update1->progress(), 5);
    }
    void testSetDownloaded()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->setDownloaded(update->identifier(), update->revision());
        QCOMPARE(m_db->get(update->identifier(), update->revision())->state(),
                 Update::State::StateDownloaded);
    }
    void testStartUpdate()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->startUpdate(update->identifier(), update->revision());
        QCOMPARE(m_db->get(update->identifier(), update->revision())->state(),
                 Update::State::StateDownloading);
    }
    void testQueueUpdate()
    {
        auto update = createUpdate("id", 42);
        QString targetDid("someId");
        m_db->add(update);
        m_model->queueUpdate(update->identifier(), update->revision(), targetDid);
        auto update1 = m_db->get(update->identifier(), update->revision());
        QCOMPARE(update1->state(), Update::State::StateQueuedForDownload);
        QCOMPARE(update1->downloadId(), targetDid);
    }
    void testProcessUpdate()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->processUpdate(update->identifier(), update->revision());
        QCOMPARE(m_db->get(update->identifier(), update->revision())->state(),
                 Update::State::StateInstalling);
    }
    void testPauseUpdate()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->pauseUpdate(update->identifier(), update->revision());
        QCOMPARE(m_db->get(update->identifier(), update->revision())->state(),
                 Update::State::StateDownloadPaused);
    }
    void testResumeUpdate()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->resumeUpdate(update->identifier(), update->revision());
        QCOMPARE(m_db->get(update->identifier(), update->revision())->state(),
                 Update::State::StateDownloading);
    }
    void testCancelUpdate()
    {
        auto update = createUpdate("id", 42);
        m_db->add(update);
        m_model->cancelUpdate(update->identifier(), update->revision());

        auto update1 = m_model->get(update->identifier(), update->revision());
        QCOMPARE(update1->error(), QString(""));
        QCOMPARE(update1->progress(), 0);
        QCOMPARE(update1->token(), QString(""));
        QCOMPARE(update1->downloadId(), QString(""));
        QCOMPARE(update1->state(), Update::State::StateAvailable);
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
        QVERIFY(names[UpdateModel::Roles::SignedDownloadUrlRole] == "signedDownloadUrl");
    }
    void testFilterKinds_data()
    {
        QTest::addColumn<Update::Kind>("kind");
        QTest::newRow("Unknown") << Update::Kind::KindUnknown;
        QTest::newRow("Click") << Update::Kind::KindClick;
        QTest::newRow("Images") << Update::Kind::KindImage;
    }
    void testFilterKinds()
    {
        QFETCH(Update::Kind, kind);

        m_filter->filterOnKind((uint) kind);
        QCOMPARE((uint) m_filter->kindFilter(), (uint) kind);
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
        QTest::addColumn<Update::Kind>("kindFilter");
        QTest::addColumn<int>("targetCount");

        QList<QSharedPointer<Update>> sample;
        sample << createClickUpdate("a", 1) << createClickUpdate("b", 2)
               << createImageUpdate("u", 1);

        QTest::newRow("Filter on clicks") << sample << Update::Kind::KindClick << 2;
        QTest::newRow("Filter on images") << sample << Update::Kind::KindImage << 1;

    }
    void testFilterKindsIntegration()
    {
        QFETCH(QList<QSharedPointer<Update> >, updates);
        QFETCH(Update::Kind, kindFilter);
        QFETCH(int, targetCount);

        Q_FOREACH(auto update, updates) {
            m_model->add(update);
        }

        m_filter->filterOnKind((uint) kindFilter);
        QCOMPARE(m_filter->rowCount(), targetCount);
    }
    void testFilteringInstalledIntegration_data()
    {
        QTest::addColumn<QList<QSharedPointer<Update>> >("updates");
        QTest::addColumn<bool>("installedFilter");
        QTest::addColumn<int>("targetCount");

        auto installed = createUpdate("a", 1);
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

        Q_FOREACH(auto update, updates) {
            m_model->add(update);
        }

        m_filter->filterOnInstalled(installedFilter);
        QCOMPARE(m_filter->rowCount(), targetCount);
    }
    void testPendingSort_data()
    {
        QTest::addColumn<QList<QSharedPointer<Update>> >("updates");
        QTest::addColumn<QStringList>("titleOrder");

        auto a = createUpdate("a", 1);
        a->setTitle("A");
        a->setInstalled(false);
        auto b = createUpdate("b", 1);
        b->setTitle("B");
        b->setInstalled(false);
        auto c = createUpdate("c", 1);
        c->setTitle("C");
        c->setInstalled(false);
        QList<QSharedPointer<Update> > updates1; updates1 << a << b << c;
        QStringList order1; order1 << "A" << "B" << "C";
        QTest::newRow("abc") << updates1 << order1;

        QList<QSharedPointer<Update> > updates2; updates2 << c << b << a;
        QTest::newRow("cba") << updates2 << order1;
    }
    void testPendingSort()
    {
        QFETCH(QList<QSharedPointer<Update>>, updates);
        QFETCH(QStringList, titleOrder);

        m_filter->filterOnInstalled(false);
        Q_FOREACH(auto update, updates) {
            m_model->add(update);
        }

        QCOMPARE(m_filter->rowCount(), 3);
        for (int i = 0; i < m_filter->rowCount(); i++) {
            QCOMPARE(
                 m_filter->data(m_filter->index(i, 0), UpdateModel::TitleRole).toString(),
                 titleOrder.at(i)
            );
        }
    }
    void testInstalledSort_data()
    {
        QTest::addColumn<QList<QSharedPointer<Update>> >("updates");
        QTest::addColumn<QStringList>("idOrder");

        auto old = createUpdate("old", 1);
        old->setUpdatedAt(QDateTime(QDate(2012, 1, 1)));
        old->setInstalled(true);
        auto older = createUpdate("older", 1);
        older->setUpdatedAt(QDateTime(QDate(2011, 1, 1)));
        older->setInstalled(true);
        auto oldest = createUpdate("oldest", 1);
        oldest->setUpdatedAt(QDateTime(QDate(2010, 1, 1)));
        oldest->setInstalled(true);
        QList<QSharedPointer<Update> > updates1; updates1 << old << older << oldest;
        QStringList order1; order1 << "old" << "older" << "oldest";
        QTest::newRow("old, older, oldest") << updates1 << order1;

        QList<QSharedPointer<Update> > updates2; updates2 << oldest << older << old;
        QTest::newRow("oldest, older, old") << updates2 << order1;
    }
    void testInstalledSort()
    {
        QFETCH(QList<QSharedPointer<Update>>, updates);
        QFETCH(QStringList, idOrder);

        m_filter->filterOnInstalled(true);
        Q_FOREACH(auto update, updates) {
            m_model->add(update);
        }

        QCOMPARE(m_filter->rowCount(), 3);
        for (int i = 0; i < m_filter->rowCount(); i++) {
            QCOMPARE(
                 m_filter->data(m_filter->index(i, 0), UpdateModel::IdRole).toString(),
                 idOrder.at(i)
            );
        }
    }
private:
    UpdateDb *m_db = nullptr;
    UpdateModel *m_model = nullptr;
    UpdateModelFilter *m_filter = nullptr;
};

QTEST_GUILESS_MAIN(TstUpdateModel)
#include "tst_updatemodel.moc"
