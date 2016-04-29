#include <QProcessEnvironment>
#include <QTest>

#include "helpers.h"

class TstUpdatePluginHelpers
    : public QObject
{
    Q_OBJECT
private slots:
    void testGetFrameworksDir()
    {
        QCOMPARE(UpdatePlugin::Helpers::getFrameworksDir(),
                env.value("FRAMEWORKS_FOLDER"));
    }
    void testGetAvailableFrameworks()
    {

    }
    void testGetArchitecture()
    {

    }
    void testArchitectureFromDpkg()
    {

    }
    void testListFolder()
    {

    }
    void testClickMetadataUrl()
    {
        QCOMPARE(UpdatePlugin::Helpers::clickMetadataUrl(),
                 env.value("URL_APPS"));
    }
    void testClickTokenUrl()
    {

    }
    void testIsIgnoringCredentials()
    {
        bool shouldIgnore = env.value("IGNORE_CREDENTIALS", "") != "";
        QCOMPARE(UpdatePlugin::Helpers::isIgnoringCredentials(), shouldIgnore);

    }
    void testWhichClick()
    {
        QCOMPARE(UpdatePlugin::Helpers::whichClick(), QLatin1String("click"));
    }
    void testWhichPkcon()
    {
        QCOMPARE(UpdatePlugin::Helpers::whichPkcon(), QLatin1String("pkcon"));
    }

private:
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
};

QTEST_MAIN(TstUpdatePluginHelpers)
#include "tst_helpers.moc"

