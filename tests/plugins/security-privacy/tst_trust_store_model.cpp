/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QTest>
#include <QSignalSpy>
#include <QtQml>

#include <core/trust/resolve.h>
#include <core/trust/store.h>

#include "trust-store-model.h"

/* mocking trust-store { */
namespace
{
namespace mock
{
struct Store: public core::trust::Store
{
    std::string m_name;
    std::list<core::trust::Request> m_allRequests;
    static std::shared_ptr<mock::Store> m_instance;

    Store()
    {
    }

    ~Store()
    {
    }

    static void setInstance(std::shared_ptr<mock::Store> store) {
        m_instance = store;
    }

    struct Query: public core::trust::Store::Query
    {
        mock::Store *m_store;
        std::list<core::trust::Request> m_requests;
        std::list<core::trust::Request>::iterator m_it;
        std::string m_applicationFilter;
        core::trust::Store::Query::Status m_status;

        Query(mock::Store *store):
            m_store(store)
        {
        }

        ~Query()
        {
        }

        void all()
        {
            m_applicationFilter.clear();
        }

        core::trust::Request current()
        {
            if (m_it == m_requests.end())
            {
                throw core::trust::Store::Query::Errors::NoCurrentResult{};
            }

            return *m_it;
        }

        void erase()
        {
        }

        void execute()
        {
            if (m_applicationFilter.empty()) {
                m_requests = m_store->m_allRequests;
            } else {
                m_requests.clear();
                for (auto it = m_store->m_allRequests.begin();
                     it != m_store->m_allRequests.end();
                     it++) {
                    if (it->from == m_applicationFilter) {
                        m_requests.push_back(*it);
                    }
                }
            }
            m_it = m_requests.begin();
            m_status = (m_it == m_requests.end()) ?
                core::trust::Store::Query::Status::eor :
                core::trust::Store::Query::Status::has_more_results;
        }

        void for_answer(core::trust::Request::Answer)
        {
        }

        void for_application_id(const std::string &id)
        {
            m_applicationFilter = id;
        }

        void for_feature(core::trust::Feature)
        {
        }

        void for_interval(const core::trust::Request::Timestamp &,
                          const core::trust::Request::Timestamp &)
        {
        }

        void next()
        {
            m_it++;
            m_status = (m_it == m_requests.end()) ?
                core::trust::Store::Query::Status::eor :
                core::trust::Store::Query::Status::has_more_results;
        }

        core::trust::Store::Query::Status status() const
        {
            return m_status;
        }
    };

    void add(const core::trust::Request &r)
    {
        m_allRequests.push_back(r);
    }

    void reset()
    {
    }

    std::shared_ptr<core::trust::Store::Query> query()
    {
        auto query =
            std::shared_ptr<core::trust::Store::Query>(new mock::Store::Query(
                this));

        return query;
    }
};
}
}

std::shared_ptr<mock::Store> mock::Store::m_instance;

std::shared_ptr<core::trust::Store> core::trust::resolve_store_in_session_with_name(
        const std::string &name)
{
    if (name.empty())
        throw Errors::ServiceNameMustNotBeEmpty{};

    mock::Store::m_instance->m_name = name;
    return mock::Store::m_instance;
}
/* } mocking trust-store */

class TrustStoreModelTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();
    void testEmpty();
    void testList_data();
    void testList();
    void testAppEnabling_data();
    void testAppEnabling();

private:
    QVariant get(const QAbstractListModel *model, int row, QString roleName);

private:
    std::shared_ptr<mock::Store> m_store;
};

QVariant TrustStoreModelTest::get(const QAbstractListModel *model, int row,
                                  QString roleName)
{
    QHash<int, QByteArray> roleNames = model->roleNames();

    int role = roleNames.key(roleName.toLatin1(), -1);
    return model->data(model->index(row), role);
}

void TrustStoreModelTest::initTestCase()
{
    qmlRegisterType<TrustStoreModel>("TrustStore.Test", 0, 1,
                                     "TrustStoreModel");
}

void TrustStoreModelTest::init()
{
    m_store = std::shared_ptr<mock::Store>(new mock::Store);
    mock::Store::setInstance(m_store);
}

void TrustStoreModelTest::cleanup()
{
    m_store.reset();
}

void TrustStoreModelTest::testEmpty()
{
    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData("import TrustStore.Test 0.1\n"
                      "TrustStoreModel {\n"
                      "  serviceName: \"storeTest\"\n"
                      "}",
                      QUrl());
    QObject *object = component.create();
    QVERIFY(object != 0);
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(object);
    QVERIFY(model != 0);

    QCOMPARE(model->rowCount(), 0);

    QCOMPARE(m_store->m_name, std::string("storeTest"));
}

void TrustStoreModelTest::testList_data()
{
    QTest::addColumn<QStringList>("appIds");
    QTest::addColumn<QList<bool> >("appGrants");
    QTest::addColumn<QStringList>("expectedAppIds");
    QTest::addColumn<QList<bool> >("expectedAppGrants");

    QTest::newRow("No repetitions") <<
        (QStringList() << "Calendar" << "Gallery" << "MyApp") <<
        (QList<bool>() << true << false << true) <<
        (QStringList() << "Calendar" << "Gallery" << "MyApp") <<
        (QList<bool>() << true << false << true);

    QTest::newRow("With repetitions") <<
        (QStringList() << "Calendar" << "Gallery" << "MyApp" << "Calendar") <<
        (QList<bool>() << true << false << true << false) <<
        (QStringList() << "Calendar" << "Gallery" << "MyApp") <<
        (QList<bool>() << false << false << true);
}

void TrustStoreModelTest::testList()
{
    QFETCH(QStringList, appIds);
    QFETCH(QList<bool>, appGrants);
    QFETCH(QStringList, expectedAppIds);
    QFETCH(QList<bool>, expectedAppGrants);

    for (int i = 0; i < appIds.count(); i++) {
        core::trust::Request r;
        r.from = appIds[i].toStdString();
        r.feature = core::trust::Feature(core::trust::Request::default_feature);
        r.answer = appGrants[i] ?
            core::trust::Request::Answer::granted : core::trust::Request::Answer::denied;
        r.when = std::chrono::system_clock::now();
        m_store->add(r);
    }

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData("import TrustStore.Test 0.1\n"
                      "TrustStoreModel {\n"
                      "  serviceName: \"storeTest\"\n"
                      "}",
                      QUrl());
    QObject *object = component.create();
    QVERIFY(object != 0);
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(object);
    QVERIFY(model != 0);

    QCOMPARE(model->rowCount(), expectedAppIds.count());

    for (int i = 0; i < model->rowCount(); i++) {
        QCOMPARE(get(model, i, "applicationId").toString(), expectedAppIds[i]);
        QCOMPARE(get(model, i, "granted").toBool(), expectedAppGrants[i]);
    }
}

void TrustStoreModelTest::testAppEnabling_data()
{
    /* In this test the model will be pre-populated with three applications:
     * - Calendar: enabled
     * - Gallery: disabled
     * - MyApp: enabled
     * We change the enabled status of each one of them at a time, and we check
     * that the results are consistent. */
    QTest::addColumn<int>("row");
    QTest::addColumn<bool>("mustEnable");
    QTest::addColumn<QStringList>("expectedEnabledApps");

    QTest::newRow("Enabling Calendar") <<
        0 << true <<
        (QStringList() << "Calendar" << "MyApp");

    QTest::newRow("Disabling Calendar") <<
        0 << false <<
        (QStringList() << "MyApp");

    QTest::newRow("Enabling Gallery") <<
        1 << true <<
        (QStringList() << "Calendar" << "Gallery" << "MyApp");

    QTest::newRow("Disabling Gallery") <<
        1 << false <<
        (QStringList() << "Calendar" << "MyApp");

    QTest::newRow("Enabling MyApp") <<
        2 << true <<
        (QStringList() << "Calendar" << "MyApp");

    QTest::newRow("Disabling MyApp") <<
        2 << false <<
        (QStringList() << "Calendar");
}

void TrustStoreModelTest::testAppEnabling()
{
    QFETCH(int, row);
    QFETCH(bool, mustEnable);
    QFETCH(QStringList, expectedEnabledApps);

    /* Pre-populate the model (update the comment on top of
     * testAppEnabling_data() if you change this). */
    QStringList apps;
    apps << "Calendar" << "Gallery" << "MyApp";
    QList<bool> appStatuses;
    appStatuses << true << false << true;
    for (int i = 0; i < apps.count(); i++) {
        core::trust::Request r;
        r.from = apps[i].toStdString();
        r.feature = core::trust::Feature(core::trust::Request::default_feature);
        r.answer = appStatuses[i] ?
            core::trust::Request::Answer::granted : core::trust::Request::Answer::denied;
        r.when = std::chrono::system_clock::now();
        m_store->add(r);
    }

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData("import TrustStore.Test 0.1\n"
                      "TrustStoreModel {\n"
                      "  serviceName: \"storeTest\"\n"
                      "}",
                      QUrl());
    QObject *object = component.create();
    QVERIFY(object != 0);
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(object);
    QVERIFY(model != 0);

    QCOMPARE(model->rowCount(), apps.count());

    QSignalSpy dataChanged(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));

    /* Enable or disable an application */
    bool ok;
    ok = QMetaObject::invokeMethod(object, "setEnabled",
                                   Q_ARG(int, row),
                                   Q_ARG(bool, mustEnable));
    QVERIFY(ok);

    QTRY_COMPARE(dataChanged.count(), 1);

    QStringList enabledApps;
    for (int i = 0; i < model->rowCount(); i++) {
        if (get(model, i, "granted").toBool()) {
            enabledApps.append(get(model, i, "applicationId").toString());
        }
    }

    QCOMPARE(enabledApps, expectedEnabledApps);
    QCOMPARE(object->property("grantedCount").toInt(),
             expectedEnabledApps.count());
}

QTEST_MAIN(TrustStoreModelTest)

#include "tst_trust_store_model.moc"
