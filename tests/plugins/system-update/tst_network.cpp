/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QTest>
#include <QScopedPointer>
#include <QString>

#include <gmock/gmock.h>

#include "mock_update.h"
#include "network.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::AnyNumber;
using ::testing::Return;

using namespace UpdatePlugin;

namespace {
    const QString NAME_KEY = "name";
    const QString VERSION_KEY = "version";
    const QString ICON_KEY = "icon_url";
    const QString DOWNLOAD_URL_KEY = "download_url";
    const QString BINARY_SIZE_KEY = "binary_filesize";
}

// helper class that exposes protected members
class PublicNetwork : public Network
{
public:
    PublicNetwork(QHash<QString, Update*> apps, QObject *parent=0) :
        Network(apps, parent) {}

    using Network::getUrlApps;
    using Network::parseUpdateObject;
};

class NetworkTest: public QObject
{
    Q_OBJECT

public:
    NetworkTest() : QObject() {};

private Q_SLOTS:
    void testParseUpdateNotObject();
    void testParseUpdateMissingName();
    void testParseUpdateMissingVersion();
    void testParseUpdateRequired_data();
    void testParseUpdateRequired();
    void testParseUpdateNotRequired();
    void testParseUpdateMissingInHash();
};

void NetworkTest::testParseUpdateNotObject()
{
    QJsonValue value(true);

    QHash<QString, Update*> updates;
    QScopedPointer<PublicNetwork> network(new PublicNetwork(updates));

    QVERIFY(!network->parseUpdateObject(value));
}

void NetworkTest::testParseUpdateMissingName()
{
    QJsonObject obj;
    QJsonValue value(obj);

    QHash<QString, Update*> updates;
    QScopedPointer<PublicNetwork> network(new PublicNetwork(updates));

    QVERIFY(!network->parseUpdateObject(value));
}

void NetworkTest::testParseUpdateMissingVersion()
{
    QJsonObject obj;
    obj["name"] = "test";
    QJsonValue value(obj);

    QHash<QString, Update*> updates;
    QScopedPointer<PublicNetwork> network(new PublicNetwork(updates));

    QVERIFY(!network->parseUpdateObject(value));
}

void NetworkTest::testParseUpdateRequired_data()
{
    QTest::addColumn<QHash<QString, Update*>>("updates");
    QTest::addColumn<QJsonValue>("json");

    QString firstName = "first-app";
    QString secondName = "second-app";

    QJsonObject firstJson;
    firstJson[NAME_KEY] = firstName;
    firstJson[VERSION_KEY] = QJsonValue(QString("2.2"));
    firstJson[ICON_KEY] = QJsonValue(QString("www.test.com/first-app-icon"));
    firstJson[DOWNLOAD_URL_KEY] = QJsonValue(QString("www.test.com/first-app"));
    firstJson[BINARY_SIZE_KEY] = QJsonValue(23.3);

    QJsonObject secondJson;
    secondJson[NAME_KEY] = secondName;
    secondJson[VERSION_KEY] = QJsonValue(QString("1.2"));
    secondJson[ICON_KEY] = QJsonValue(QString("www.test.com/second-app-icon"));
    secondJson[DOWNLOAD_URL_KEY] = QJsonValue(QString("www.test.com/second-app"));
    secondJson[BINARY_SIZE_KEY] = QJsonValue(3.3);

    QHash<QString, Update*> singleUpdate;
    singleUpdate[firstName] = new MockedUpdate();

    QHash<QString, Update*> severalUpdates;
    severalUpdates[firstName] = new MockedUpdate();
    severalUpdates[secondName] = new MockedUpdate();

    QTest::newRow("single-obj") << singleUpdate << QJsonValue(firstJson);
    QTest::newRow("several-obj") << severalUpdates << QJsonValue(secondJson);
}

void NetworkTest::testParseUpdateRequired()
{
    // the typedef is needed so that QFETCH does work
    typedef QHash<QString, Update*> UpdatesHash;
    QFETCH(UpdatesHash, updates);
    QFETCH(QJsonValue, json);

    auto jsonObj = json.toObject();
    auto jsonName = jsonObj[NAME_KEY].toString();
    if (jsonName.isEmpty()) {
        QFAIL("Wrong test data.");
    }

    auto version = jsonObj[VERSION_KEY].toString();
    auto icon_url = jsonObj[ICON_KEY].toString();
    auto url = jsonObj[DOWNLOAD_URL_KEY].toString();
    auto size = jsonObj[BINARY_SIZE_KEY].toInt();

    // set the expectations of the updates, only the update with the same name
    // as the json will have the methods called once, the rest should not be
    // called at all
    foreach(const QString& name, updates.keys()) {
	auto update = qobject_cast<MockedUpdate*>(updates[name]);
        if (name == jsonName) {
            // set expectations
            EXPECT_CALL(*update, setRemoteVersion(version))
                .Times(1);

            EXPECT_CALL(*update, updateRequired())
                .Times(1)
                .WillOnce(Return(true));

            EXPECT_CALL(*update, setIconUrl(icon_url))
                .Times(1);

            EXPECT_CALL(*update, setDownloadUrl(url))
                .Times(1);

            EXPECT_CALL(*update, setBinaryFilesize(size))
                .Times(1);
	} else {
            // methods most not be called
            EXPECT_CALL(*update, setRemoteVersion(version))
                .Times(0);

            EXPECT_CALL(*update, updateRequired())
                .Times(0);

            EXPECT_CALL(*update, setIconUrl(icon_url))
                .Times(0);

            EXPECT_CALL(*update, setDownloadUrl(url))
                .Times(0);

            EXPECT_CALL(*update, setBinaryFilesize(size))
                .Times(0);
	}
    }

    QScopedPointer<PublicNetwork> network(new PublicNetwork(updates));
    QVERIFY(network->parseUpdateObject(json));

    // verify each of the mocked objects
    foreach(Update* update, updates) {
	auto mock = qobject_cast<MockedUpdate*>(update);
        QVERIFY(Mock::VerifyAndClearExpectations(mock));
    }
    // we must delete all the mocks
    qDeleteAll(updates);
}

void NetworkTest::testParseUpdateNotRequired()
{
    // create updates with a mock obj that does not need an update
    QString appName = "first-app";

    QJsonObject json;
    json[NAME_KEY] = appName;
    json[VERSION_KEY] = QJsonValue(QString("2.2"));
    json[ICON_KEY] = QJsonValue(QString("www.test.com/first-app-icon"));
    json[DOWNLOAD_URL_KEY] = QJsonValue(QString("www.test.com/first-app"));
    json[BINARY_SIZE_KEY] = QJsonValue(23.3);

    auto update = new MockedUpdate();
    QHash<QString, Update*> updates;
    updates[appName] = update;

    // set expectations
    EXPECT_CALL(*update, setRemoteVersion(_))
        .Times(1);

    EXPECT_CALL(*update, updateRequired())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*update, setIconUrl(_))
        .Times(0);

    EXPECT_CALL(*update, setDownloadUrl(_))
        .Times(0);

    EXPECT_CALL(*update, setBinaryFilesize(_))
        .Times(0);

    QScopedPointer<PublicNetwork> network(new PublicNetwork(updates));
    QVERIFY(!network->parseUpdateObject(json));
    QVERIFY(Mock::VerifyAndClearExpectations(update));
    qDeleteAll(updates);
}

void NetworkTest::testParseUpdateMissingInHash()
{
    // send an update for a missing app
    QString appName = "first-app";

    QJsonObject json;
    json[NAME_KEY] = appName;
    json[VERSION_KEY] = QJsonValue(QString("2.2"));
    json[ICON_KEY] = QJsonValue(QString("www.test.com/first-app-icon"));
    json[DOWNLOAD_URL_KEY] = QJsonValue(QString("www.test.com/first-app"));
    json[BINARY_SIZE_KEY] = QJsonValue(23.3);

    QHash<QString, Update*> updates;

    QScopedPointer<PublicNetwork> network(new PublicNetwork(updates));
    QVERIFY(!network->parseUpdateObject(json));
}

QTEST_MAIN(NetworkTest)
#include "tst_network.moc"
