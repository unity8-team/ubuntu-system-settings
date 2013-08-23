#include <QtQml>
#include <QtQml/QQmlContext>
#include "plugin.h"
#include "wifi.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.TimeDate"));

    qmlRegisterType<TimeDate>(uri, 1, 0, "UbuntuTimeDatePanel");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
