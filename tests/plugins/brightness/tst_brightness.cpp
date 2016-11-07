#include "brightness.h"
#include "fakemirclient.h"

#include <QSignalSpy>
#include <QTest>

using namespace DisplayPlugin;

class TstBrightnessConstruction : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
    }
    void cleanupTestCase()
    {
    }
    void init()
    {
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    void test_null_config_while_connected()
    {
        m_mirClient = new FakeMirClient();
        m_mirClient->connected = true;
        m_mirClient->conf = nullptr;
        m_instance = new Brightness(QDBusConnection::systemBus(), m_mirClient);
        QCOMPARE(m_instance->allDisplays()->rowCount(), 0);
    }
    void test_not_connected()
    {
        m_mirClient = new FakeMirClient();
        m_mirClient->connected = false;
        m_instance = new Brightness(QDBusConnection::systemBus(), m_mirClient);
        QCOMPARE(m_instance->allDisplays()->rowCount(), 0);
    }
    void test_config()
    {
        m_mirClient = new FakeMirClient();
        m_mirClient->connected = true;

        MirDisplayConfiguration conf;
        conf.num_outputs = 1;
        MirDisplayOutput output;
        MirDisplayOutput outputs[1];
        conf.outputs = outputs;
        conf.outputs[0] = output;

        m_mirClient->conf = &conf;
        m_instance = new Brightness(QDBusConnection::systemBus(), m_mirClient);
        QCOMPARE(m_instance->allDisplays()->rowCount(), 1);
    }
private:
    FakeMirClient *m_mirClient = nullptr;
    Brightness *m_instance = nullptr;
};

QTEST_GUILESS_MAIN(TstBrightnessConstruction)
#include "tst_brightness.moc"

