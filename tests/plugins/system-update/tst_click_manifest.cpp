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

/*
 * This test uses a mock web server.
 */

#include "click/manifest_impl.h"

#include <QSignalSpy>
#include <QTest>
#include <QUrl>

#include <QDebug>

using namespace UpdatePlugin;

class TstClickManifest
    : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_instance = new Click::ManifestImpl();
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    void testRequestSucceeded()
    {
        QSignalSpy requestSucceededSpy(m_instance, SIGNAL(requestSucceeded(const QJsonArray&)));
        m_instance->request();
        QVERIFY(requestSucceededSpy.wait());
        QList<QVariant> args = requestSucceededSpy.takeFirst();
        QJsonArray res = args.at(0).toJsonArray();
        QCOMPARE(res.size(), 4); // See click.result
    }
private:
    Click::ManifestImpl *m_instance = nullptr;
};

QTEST_MAIN(TstClickManifest)
#include "tst_click_manifest.moc"

