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

#include "click/apiclient_impl.h"
#include "network/accessmanager_impl.h"

#include "mockclickserver.h"

#include <QJsonArray>
#include <QSignalSpy>
#include <QTest>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>

using namespace UpdatePlugin;

class TstClickApiClient
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
        m_nam = new Network::ManagerImpl();
        m_instance = new Click::ApiClientImpl(m_nam, this);
        m_nam->setParent(m_instance);
    }
    void cleanup()
    {
        m_instance->cancel();
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    void testTokenRequestSuccess()
    {
        QSignalSpy tokenRequestSucceededSpy(m_instance, SIGNAL(tokenRequestSucceeded(const QString)));
        QUrl query("http://localhost:9009/download");
        m_instance->requestToken(query);
        QVERIFY(tokenRequestSucceededSpy.wait());
        QList<QVariant> args = tokenRequestSucceededSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), QString("Mock-X-Click-Token"));
    }
    void testTokenRequestFailure()
    {
        QSignalSpy credentialErrorSpy(m_instance, SIGNAL(tokenRequestSucceeded(const QString)));
        QUrl query("http://localhost:9009/403");
        m_instance->requestToken(query);
        QVERIFY(credentialErrorSpy.wait());
        QCOMPARE(credentialErrorSpy.count(), 1);
    }
    void testMetadataRequestSuccess_data()
    {
        QTest::addColumn<QList<QString> >("names");

        QList<QString> none;
        QTest::newRow("Empty") << none;

        QList<QString> one; one << "name";
        QTest::newRow("One") << one;

        QList<QString> many; many << "first" << "second" << "third" << "fourth";
        QTest::newRow("Many") << many;
    }
    void testMetadataRequestSuccess()
    {
        QFETCH(QList<QString>, names);
        QSignalSpy metadataRequestSucceededSpy(
            m_instance, SIGNAL(metadataRequestSucceeded(const QJsonArray&))
        );
        QUrl query("http://localhost:9009/metadata");
        m_instance->requestMetadata(query, names);
        QVERIFY(metadataRequestSucceededSpy.wait());
        QCOMPARE(metadataRequestSucceededSpy.count(), 1);
    }
    void testMetadataRequestAuthFailure()
    {
        QSignalSpy credentialErrorSpy(m_instance, SIGNAL(credentialError()));
        QUrl query("http://localhost:9009/403");
        m_instance->requestMetadata(query, QList<QString>());
        QVERIFY(credentialErrorSpy.wait());
        QCOMPARE(credentialErrorSpy.count(), 1);
    }
    void testMetadataRequestNotFoundFailure()
    {
        QSignalSpy serverErrorSpy(m_instance, SIGNAL(serverError()));
        QUrl query("http://localhost:9009/404");
        m_instance->requestMetadata(query, QList<QString>());
        QVERIFY(serverErrorSpy.wait());
        QCOMPARE(serverErrorSpy.count(), 1);
    }
    void testTokenRequestDispatching()
    {
        Click::ApiClientImpl *a = new Click::ApiClientImpl(m_nam);
        Click::ApiClientImpl *b = new Click::ApiClientImpl(m_nam);

        // We only want “a” to receive a signal.
        QSignalSpy aSuccessSpy(a, SIGNAL(tokenRequestSucceeded(const QString)));
        QSignalSpy bSuccessSpy(b, SIGNAL(tokenRequestSucceeded(const QString)));
        QUrl query("http://localhost:9009/download");
        a->requestToken(query);
        QVERIFY(aSuccessSpy.wait());
        QCOMPARE(aSuccessSpy.count(), 1);
        QCOMPARE(bSuccessSpy.count(), 0);

        a->deleteLater();
        b->deleteLater();
    }
    void testMetadataRequestDispatching()
    {
        Click::ApiClientImpl *a = new Click::ApiClientImpl(m_nam);
        Click::ApiClientImpl *b = new Click::ApiClientImpl(m_nam);

        // We only want “a” to receive a signal.
        QSignalSpy aSuccessSpy(
            a, SIGNAL(metadataRequestSucceeded(const QJsonArray&))
        );
        QSignalSpy bSuccessSpy(
            b, SIGNAL(metadataRequestSucceeded(const QJsonArray&))
        );
        QUrl query("http://localhost:9009/metadata");
        a->requestMetadata(query, QList<QString>());
        QVERIFY(aSuccessSpy.wait());
        QCOMPARE(aSuccessSpy.count(), 1);
        QCOMPARE(bSuccessSpy.count(), 0);

        a->deleteLater();
        b->deleteLater();
    }
private:
    Click::ApiClient *m_instance = nullptr;
    Network::Manager *m_nam = nullptr;
};

QTEST_MAIN(TstClickApiClient)
#include "tst_clickclient.moc"

