#ifndef MIRDISPLAYS_IMPL_H
#define MIRDISPLAYS_IMPL_H

#include "mirdisplays.h"

class MirDisplaysImpl : public MirDisplays
{
    Q_OBJECT
public:
    explicit MirDisplaysImpl(QObject *parent = 0);
    ~MirDisplaysImpl();
    virtual MirDisplayConfiguration* getConfiguration() const;
    virtual void setConfiguration(MirDisplayConfiguration *conf) override;
    virtual bool applyConfiguration(MirDisplayConfiguration *conf) override;
    virtual bool isConnected() override;

private:
    void connect();
    MirConnection *m_mir_connection;
    MirDisplayConfiguration *m_configuration;
};

#endif // MIRDISPLAYS_IMPL_H
