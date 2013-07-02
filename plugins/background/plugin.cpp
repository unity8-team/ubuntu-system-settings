#include <QtQml>
#include <QtQml/QQmlContext>
#include "plugin.h"
#include "background.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Background"));
    
    qmlRegisterType<Background>(uri, 1, 0, "UbuntuBackgroundPanel");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
