#include "brightness.h"
#include "fakemirclient.h"

#include <QSignalSpy>
#include <QTest>

using namespace DisplayPlugin;

class TstBrightness : public QObject
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
        m_mirClient = new FakeMirClient();
        m_instance = new Brightness(QDBusConnection::systemBus(), m_mirClient);
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    void test_foo()
    {

    }
private:
    MirClient *m_mirClient = nullptr;
    Brightness *m_instance = nullptr;
};

QTEST_GUILESS_MAIN(TstBrightness)
#include "tst_brightness.moc"

