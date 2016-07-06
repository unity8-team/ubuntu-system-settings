#ifndef MOCK_CLICKSERVER_TESTCASE_H
#define MOCK_CLICKSERVER_TESTCASE_H

#include <QTest>
#include <QProcess>

class MockClickServerTestCase
{
protected:
    void startMockClickServer(const QStringList &args = QStringList())
    {
        QStringList params;
        params << "mockclickserver.py";
        params << args;
        m_mockclickserver.start("python3", params);
        QTRY_COMPARE(m_mockclickserver.state(), QProcess::Running);
    }

    void stopMockClickServer()
    {
        m_mockclickserver.close();
        QTRY_COMPARE(m_mockclickserver.state(), QProcess::NotRunning);
    }

    QProcess m_mockclickserver;
};

#endif // MOCK_CLICKSERVER_TESTCASE_H
