#ifndef MOCK_CLICKSERVER_TESTCASE_H
#define MOCK_CLICKSERVER_TESTCASE_H

#include <QTest>
#include <QProcess>
#include <QDebug>

class MockClickServerTestCase
{
protected:
    void startMockClickServer(const QStringList &args = QStringList())
    {
        QStringList params;
        params << "mockclickserver.py";
        params << args;
        m_mockclickserver.start("python3", params);
        QVERIFY(m_mockclickserver.waitForStarted());

        QTRY_COMPARE(m_mockclickserver.state(), QProcess::Running);

        /* QTRY_VERIFY can't be used for reading std out for some
        reason, so we do a manual timeout on failure to read
        “Started” from the server’s output. */
        for (int i = 0; i < 5; i++) {
            QString out(m_mockclickserver.readAllStandardOutput());
            if (out.contains("Started")) {
                return;
            }
            QTest::qWait(1000);
        }
        QFAIL("could not start mock server in time.");
    }

    void stopMockClickServer()
    {
        m_mockclickserver.close();
        QTRY_COMPARE(m_mockclickserver.state(), QProcess::NotRunning);
    }

    QProcess m_mockclickserver;
};

#endif // MOCK_CLICKSERVER_TESTCASE_H
