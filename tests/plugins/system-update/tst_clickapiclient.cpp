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
        QSignalSpy readyReadStandardOutputSpy(&m_mockclickserver,
            SIGNAL(readyReadStandardOutput()));
        QVERIFY(readyReadStandardOutputSpy.wait());
    }
    void cleanupTestCase()
    {
        m_mockclickserver.close();
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

protected:
    void requestSucceeded(QNetworkReply *reply)
    {

    }

private:
    QNetworkReply* request(const QString &url) {
        QUrl u(url);
        QNetworkRequest request;
        request.setUrl(u);

        QNetworkReply *reply = m_nam.get(request);
        initializeReply(reply);
        return reply;
    }
};

QTEST_MAIN(TstClickApiClient)
#include "tst_clickapiclient.moc"

