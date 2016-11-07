#ifndef FAKEMIRCLIENT_H
#define FAKEMIRCLIENT_H

#include "mirclient.h"

#include <mir_toolkit/mir_client_library.h>
#include <QObject>

class FakeMirClient : public DisplayPlugin::MirClient
{
    Q_OBJECT
public:
    explicit FakeMirClient(QObject *parent = 0)
        : DisplayPlugin::MirClient(parent) {}
    virtual ~FakeMirClient() {}
    virtual MirDisplayConfiguration* getConfiguration() const override
    {
        return conf;
    }
    virtual void setConfiguration(MirDisplayConfiguration *conf) override
    {
    }
    virtual bool applyConfiguration(MirDisplayConfiguration *conf) override
    {
        // This usually happens via a mir callback. Fake it here.
        this->conf = conf;
        Q_EMIT configurationChanged();
    }
    virtual bool isConnected() override
    {
        return connected;
    }

    bool connected = false;
    MirDisplayConfiguration *conf = nullptr;
};

#endif // FAKEMIRCLIENT_H
