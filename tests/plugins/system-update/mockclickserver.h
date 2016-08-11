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
        params << "--port" << QString::number(m_port);
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

        qWarning() << "Failed to start server"
                   << m_mockclickserver.readAllStandardError()
                   << "\n\nRetrying on a different port...";

        if (m_retries < 10) {
            m_port++;
            m_retries++;
            startMockClickServer(args);
        } else {
            QFAIL("Could not start server.");
        }
    }

    void stopMockClickServer()
    {
        m_mockclickserver.close();
        QTRY_COMPARE(m_mockclickserver.state(), QProcess::NotRunning);
    }

    QProcess m_mockclickserver;
    uint m_port = 9009;
    uint m_retries = 0;
};

#endif // MOCK_CLICKSERVER_TESTCASE_H
