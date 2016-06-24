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
#include <QFile>
#include <QSignalSpy>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTime>
#include <QTimeZone>
#include <QTest>

#include "clickupdatemetadata.h"
#include "updatemodel.h"

using namespace UpdatePlugin;

class TstUpdateModel : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_dbfile = m_dir->path() + "/cupdatesstore.db";
        m_model = new UpdateModel(m_dbfile);
    }
    void cleanup()
    {
        delete m_model;
        delete m_dir;
    }
    void testNoUpdates()
    {
        QCOMPARE(m_model->count(), 0);
    }
    void testUpdate()
    {
        ClickUpdateMetadata m;
        m.setName("test.app");
        m.setRevision(1);

        m_model->add(&m);
        QTest::qWait(550);
        m_model->refresh();
        QTest::qWait(550);

        QCOMPARE(m_model->count(), 1);
    }
    void testFilters()
    {
        ClickUpdateMetadata pendingApp;
        pendingApp.setName("pending.app");
        pendingApp.setRevision(1);

        ClickUpdateMetadata installedApp;
        installedApp.setName("installed.app");
        installedApp.setRevision(1);

        m_model->add(&pendingApp);
        m_model->add(&installedApp);
        m_model->setDownloadId("installed.app", 1, "1234");
        m_model->setInstalled("1234");

        m_model->setFilter(UpdateModel::UpdateTypes::PendingClicksUpdates);
        QCOMPARE(m_model->count(), 1);
        QCOMPARE(
            m_model->data(
                m_model->index(0), UpdateModel::IdRole
            ).toString(), pendingApp.name()
        );

        m_model->setFilter(UpdateModel::UpdateTypes::InstalledClicksUpdates);
        QCOMPARE(m_model->count(), 1);

        QCOMPARE(
            m_model->data(
                m_model->index(0), UpdateModel::IdRole
            ).toString(),
            installedApp.name()
        );

        m_model->setFilter(UpdateModel::UpdateTypes::All);
        QCOMPARE(m_model->count(), 2);
    }
    void testSupersededUpdate()
    {
        ClickUpdateMetadata superseded;
        superseded.setName("some.app");
        superseded.setRevision(1);

        ClickUpdateMetadata replacement;
        replacement.setName("some.app");
        replacement.setRevision(2);

        m_model->add(&superseded);
        m_model->add(&replacement);

        // We only want the replacement in our model of pending updates.
        m_model->setFilter(UpdateModel::UpdateTypes::PendingClicksUpdates);
        QCOMPARE(m_model->count(), 1);
        QCOMPARE(m_model->data(
            m_model->index(0), UpdateModel::IdRole
        ).toString(), replacement.name());
    }
    void testRoles()
    {
        return;
        using namespace UpdatePlugin;
        ClickUpdateMetadata app;
        QStringList mc;
        mc << "ls" << "la";

        app.setName("com.ubuntu.testapp");
        app.setLocalVersion("0.1");
        app.setRemoteVersion("0.2");
        app.setRevision(42);
        app.setTitle("Test App");
        app.setDownloadSha512("987654323456789");
        app.setBinaryFilesize(1000);
        app.setIconUrl("http://example.org/testapp.png");
        app.setDownloadUrl("http://example.org/testapp.click");
        app.setCommand(mc);
        app.setChangelog("* Fixed all bugs * Introduced new bugs");
        app.setClickToken("Mock-X-Click-Token");

        QSignalSpy storeChangedSpy(m_model, SIGNAL(changed()));
        m_model->add(&app);
        m_model->refresh();
        QTRY_COMPARE(storeChangedSpy.count(), 1);
        // m_model->setState(app.name(), app.revision(),
        //                         SystemUpdate::UpdateState::StateAvailable);
        // m_model->setProgress(app.name(), app.revision(), 50);

        // m_model->updateItem(app.name(), app.revision());

        QModelIndex idx = m_model->index(0);

        QCOMPARE(m_model->data(idx, UpdateModel::KindRole).toString(), UpdateModel::KIND_CLICK);
        QCOMPARE(m_model->data(idx, UpdateModel::IdRole).toString(), app.name());
        QCOMPARE(m_model->data(idx, UpdateModel::LocalVersionRole).toString(), app.localVersion());
        QCOMPARE(m_model->data(idx, UpdateModel::RemoteVersionRole).toString(), app.remoteVersion());
        QCOMPARE(m_model->data(idx, UpdateModel::RevisionRole).toInt(), app.revision());
        QCOMPARE(m_model->data(idx, UpdateModel::TitleRole).toString(), app.title());
        QCOMPARE(m_model->data(idx, UpdateModel::DownloadHashRole).toString(), app.downloadSha512());
        QCOMPARE(m_model->data(idx, UpdateModel::SizeRole).toUInt(), app.binaryFilesize());
        QCOMPARE(m_model->data(idx, UpdateModel::IconUrlRole).toString(), app.iconUrl());
        QCOMPARE(m_model->data(idx, UpdateModel::DownloadUrlRole).toString(), app.downloadUrl());
        QCOMPARE(m_model->data(idx, UpdateModel::CommandRole).toStringList(), app.command());
        QCOMPARE(m_model->data(idx, UpdateModel::ChangelogRole).toString(), app.changelog());
        QCOMPARE(m_model->data(idx, UpdateModel::StateRole).toString(), UpdateModel::STATE_PENDING);
        QCOMPARE(m_model->data(idx, UpdateModel::AutomaticRole).toBool(), app.automatic());

        int stateInt = m_model->data(idx, UpdateModel::UpdateStateRole).toInt();
        SystemUpdate::UpdateState u = (SystemUpdate::UpdateState) stateInt;
        QCOMPARE(u, SystemUpdate::UpdateState::StateAvailable);

        QCOMPARE(m_model->data(idx, UpdateModel::ProgressRole).toInt(), 50);

        // Verify that the date ain't empty.
        QVERIFY(!m_model->data(idx, UpdateModel::CreatedAtRole).toString().isEmpty());
    }
    void testCount()
    {
        // TODO implement!!
    }
private:
    UpdateModel *m_model;
    QTemporaryDir *m_dir;
    QString m_dbfile;
};

QTEST_MAIN(TstUpdateModel)

#include "tst_updatemodel.moc"
