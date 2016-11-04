#include <QSignalSpy>
#include <QTest>

using namespace DisplayPlugin;

class TstDisplayModel : public QObject
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
        m_instance = nullptr;
    }
    void cleanup()
    {
        // QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        // m_instance->deleteLater();
        // QTRY_COMPARE(destroyedSpy.count(), 1);
    }
private:
    QObject *m_instance = nullptr;
};

QTEST_GUILESS_MAIN(TstDisplayModel)
#include "tst_displayplugin_displaymodel.moc"

