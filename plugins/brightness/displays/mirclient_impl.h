#ifndef MIRCLIENT_IMPL_H
#define MIRCLIENT_IMPL_H

#include "mirclient.h"

namespace DisplayPlugin
{
class MirClientImpl : public MirClient
{
    Q_OBJECT
public:
    explicit MirClientImpl(QObject *parent = 0);
    ~MirClientImpl();
    virtual MirDisplayConfiguration* getConfiguration() const;
    virtual void setConfiguration(MirDisplayConfiguration *conf) override;
    virtual bool applyConfiguration(MirDisplayConfiguration *conf) override;
    virtual bool isConnected() override;

private:
    void connect();
    MirConnection *m_mir_connection;
    MirDisplayConfiguration *m_configuration;
};
} // DisplayPlugin

#endif // MIRCLIENT_IMPL_H
