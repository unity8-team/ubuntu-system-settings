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
        QString target("ubuntu-sdk-15.04");
        for(auto const& value: UpdatePlugin::Helpers::getAvailableFrameworks())
             QCOMPARE(QString::fromStdString(value), target);
    }
    void testGetArchitecture()
    {
        QString res = QString::fromStdString(
            UpdatePlugin::Helpers::getArchitecture());
        QVERIFY(!res.isEmpty());
    }
    void testClickMetadataUrl()
    {
        QCOMPARE(UpdatePlugin::Helpers::clickMetadataUrl(),
                 env.value("URL_APPS"));
    }
    void testClickTokenUrl()
    {
        QString target("http://example.org");
        QCOMPARE(UpdatePlugin::Helpers::clickTokenUrl(target),
                 target);
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

