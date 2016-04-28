#ifndef MOCKCLICKSERVERTESTCASE_H_
#define MOCKCLICKSERVERTESTCASE_H_

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
    }

    QProcess m_mockclickserver;
};

#endif // MOCKCLICKSERVERTESTCASE_H_
