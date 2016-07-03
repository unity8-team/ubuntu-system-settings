#ifndef MOCK_CLICKSERVER_TESTCASE_H
#define MOCK_CLICKSERVER_TESTCASE_H

#include <QTest>
#include <QProcess>
#include <QSignalSpy>

class MockClickServerTestCase
{
protected:
    void startMockClickServer(const QStringList &args = QStringList())
    {
        QStringList params;
        params << "mock_update_click_server.py";
        params << args;
        m_mockclickserver.start("python3", params);
        QSignalSpy readyReadStandardOutputSpy(&m_mockclickserver,
            SIGNAL(readyReadStandardOutput()));
        QVERIFY(readyReadStandardOutputSpy.wait());
    }

    void stopMockClickServer()
    {
        m_mockclickserver.close();
        QTest::qWait(1000);
    }

    QProcess m_mockclickserver;
};

#endif // MOCK_CLICKSERVER_TESTCASE_H
