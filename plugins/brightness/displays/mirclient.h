#ifndef MIRCLIENT_H
#define MIRCLIENT_H

#include <mir_toolkit/mir_client_library.h>
#include <QObject>

namespace DisplayPlugin
{
class MirClient : public QObject
{
    Q_OBJECT
public:
    explicit MirClient(QObject *parent = 0) : QObject(parent) {}
    virtual ~MirClient() {};
    virtual MirDisplayConfiguration* getConfiguration() const = 0;
    virtual void setConfiguration(MirDisplayConfiguration *conf) = 0;
    virtual bool applyConfiguration(MirDisplayConfiguration *conf) = 0;
    virtual bool isConnected() = 0;

Q_SIGNALS:
    void configurationChanged() const;
};
} // DisplayPlugin

#endif // MIRCLIENT_H
