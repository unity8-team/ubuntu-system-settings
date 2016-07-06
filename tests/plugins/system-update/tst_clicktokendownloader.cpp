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


#include "clicktokendownloader.h"
#include "mockclickservertestcase.h"
#include "update.h"

#include <QSignalSpy>
#include <QTest>

using namespace UpdatePlugin;

class TstClickTokenDownloader
    : public QObject
    , public MockClickServerTestCase
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        startMockClickServer();
    }
    void cleanupTestCase()
    {
        stopMockClickServer();
    }
    void init()
    {
        m_update = new Update();
        m_instance = new ClickTokenDownloader(nullptr, m_update);
    }
    void cleanup()
    {
        delete m_instance;
        delete m_update;
    }
    void testObtainClickToken()
    {
        QSignalSpy tokenSpy(m_update, SIGNAL(tokenChanged()));

        std::vector<Update*> updates;
        QObject::connect(
            m_instance, &ClickTokenDownloader::tokenRequestSucceeded,
            [&](Update *value) {
                updates.emplace_back(value);
            }
        );

        m_update->setDownloadUrl("http://localhost:9009/download");
        m_instance->requestToken();
        QVERIFY(tokenSpy.wait());
        QCOMPARE(m_update->token(), QString("Mock-X-Click-Token"));

        QCOMPARE((int) updates.size(), 1);
        QCOMPARE(updates.at(0), m_update);
    }
    void testEmptyDownloadUrl()
    {
        std::vector<Update*> updates;
        QObject::connect(
            m_instance, &ClickTokenDownloader::tokenRequestFailed,
            [&](Update *value) {
                updates.emplace_back(value);
            }
        );
        m_instance->requestToken();

        QTRY_COMPARE((int) updates.size(), 1);
        QTRY_COMPARE(updates.at(0), m_update);
    }
    void testFailure()
    {
        stopMockClickServer();

        std::vector<Update*> updates;
        QObject::connect(
            m_instance, &ClickTokenDownloader::tokenRequestFailed,
            [&](Update *value) {
                updates.emplace_back(value);
            }
        );
        m_update->setDownloadUrl("http://localhost:9009/download");
        m_instance->requestToken();

        QTRY_COMPARE((int) updates.size(), 1);
        QTRY_COMPARE(updates.at(0), m_update);

        startMockClickServer();
    }
private:
    ClickTokenDownloader *m_instance;
    Update *m_update;
};

QTEST_MAIN(TstClickTokenDownloader)
#include "tst_clicktokendownloader.moc"

