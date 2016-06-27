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

#include "mockclickservertestcase.h"

#include "clicktokendownloader.h"

using namespace UpdatePlugin;

class TstClickTokenDownloader
    : public QObject
    , public MockClickServerTestCase
{
    Q_OBJECT
private slots:
    // void initTestCase()
    // {
    //     startMockClickServer();
    // }
    // void cleanupTestCase()
    // {
    //     m_mockclickserver.close();
    // }
    // void init()
    // {
    //     m_instance = new Click();
    // }
    // void cleanup()
    // {
    //     delete m_instance;
    // }
    // void testObtainClickToken()
    // {
    //     QSignalSpy tokenSpy(m_instance, SIGNAL(tokenChanged()));

    //     std::vector<const ClickUpdate*> updates;
    //     QObject::connect(
    //         m_instance, &ClickUpdate::clickTokenRequestSucceeded,
    //         [&](const ClickUpdate *value) {
    //             updates.emplace_back(value);
    //         }
    //     );

    //     m_instance->setDownloadUrl("http://localhost:9009/download");
    //     m_instance->requestClickToken();
    //     QVERIFY(tokenSpy.wait());
    //     QVERIFY(m_instance->token() == "Mock-X-Click-Token");

    //     // Make sure we got a meta from the tokenRequestSucceeded signal,
    //     // and that the instance we got is identical to ours.
    //     QVERIFY(updates.size() == 1);
    //     QVERIFY(updates.at(0) == m_instance);
    // }
    // void testEmptyDownloadUrl()
    // {
    //     std::vector<const ClickUpdate*> updates;
    //     QObject::connect(
    //         m_instance, &ClickUpdate::clickTokenRequestFailed,
    //         [&](const ClickUpdate *value) {
    //             updates.emplace_back(value);
    //         }
    //     );
    //     m_instance->requestClickToken();

    //     // Make sure we got a meta from the tokenRequestSucceeded signal,
    //     // and that the instance we got is identical to ours.
    //     QVERIFY(updates.size() == 1);
    //     QVERIFY(updates.at(0) == m_instance);
    // }
private:
    // ClickUpdate *m_instance;
};

QTEST_MAIN(TstClickTokenDownloader)
#include "tst_clicktokendownloader.moc"

