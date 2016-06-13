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

class TstUpdateModel : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_dbfile = m_dir->path() + "/cupdatesstore.db";
        m_model = new UpdatePlugin::UpdateModel(m_dbfile);
        m_store = new UpdatePlugin::UpdateStore(m_dbfile);
    }
    void cleanup()
    {
        m_store->db().close();
        delete m_model;
        delete m_store;
        delete m_dir;
    }
    void testNoUpdates()
    {
        QCOMPARE(m_model->count(), 0);
    }
    void testUpdate()
    {
        UpdatePlugin::ClickUpdateMetadata m;
        m.setName("test.app");
        m.setRevision(1);

        m_store->add(&m);
        m_model->update();

        QCOMPARE(m_model->count(), 1);
    }
    void testFilters()
    {
        UpdatePlugin::ClickUpdateMetadata pendingApp;
        pendingApp.setName("pending.app");
        pendingApp.setRevision(1);

        UpdatePlugin::ClickUpdateMetadata installedApp;
        installedApp.setName("installed.app");
        installedApp.setRevision(1);

        m_store->add(&pendingApp);
        m_store->add(&installedApp);
        m_store->markInstalled("installed.app", 1);

        m_model->setFilter(UpdatePlugin::UpdateModel::UpdateTypes::PendingClicksUpdates);
        QCOMPARE(m_model->count(), 1);
        // Col 1 is app_id
        QCOMPARE(m_model->data(m_model->index(0, 1), Qt::DisplayRole).toString(), pendingApp.name());

        m_model->setFilter(UpdatePlugin::UpdateModel::UpdateTypes::InstalledClicksUpdates);
        QCOMPARE(m_model->count(), 1);
        // Col 1 is app_id
        QCOMPARE(m_model->data(m_model->index(0, 1), Qt::DisplayRole).toString(), installedApp.name());

        m_model->setFilter(UpdatePlugin::UpdateModel::UpdateTypes::All);
        QCOMPARE(m_model->count(), 2);
    }
private:
    UpdatePlugin::UpdateModel *m_model;
    UpdatePlugin::UpdateStore *m_store;
    QTemporaryDir *m_dir;
    QString m_dbfile;
};

QTEST_MAIN(TstUpdateModel)

#include "tst_updatemodel.moc"
