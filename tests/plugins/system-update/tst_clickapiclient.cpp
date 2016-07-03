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

#include "clickapiclient.h"

class TstClickApiClient
    : public UpdatePlugin::ClickApiClient
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
    }
    void cleanup()
    {
        cancel();
    }
    void testForbiddenBehaviour()
    {
        QSignalSpy credentialErrorSpy(this, SIGNAL(credentialError()));
        QNetworkReply *reply = request("http://localhost:9009/403");
        QVERIFY(credentialErrorSpy.wait());
    }
    void testNotFound()
    {
        QSignalSpy serverErrorSpy(this, SIGNAL(serverError()));
        QNetworkReply *reply = request("http://localhost:9009/404");
        QVERIFY(serverErrorSpy.wait());
    }
    void testOffline()
    {
        m_mockclickserver.close();

        QSignalSpy networkErrorSpy(this, SIGNAL(networkError()));
        QNetworkReply *reply = request("http://localhost:9009/");
        QVERIFY(networkErrorSpy.wait());

        startMockClickServer();
        QSignalSpy readyReadStandardOutputSpy(&m_mockclickserver,
            SIGNAL(readyReadStandardOutput()));
        QVERIFY(readyReadStandardOutputSpy.wait());
    }

private:
    QNetworkReply* request(const QString &url) {
        QUrl u(url);
        QNetworkRequest request;
        request.setUrl(u);

        QNetworkReply *reply = nam()->get(request);
        initializeReply(reply);
        return reply;
    }
};

QTEST_MAIN(TstClickApiClient)
#include "tst_clickapiclient.moc"

