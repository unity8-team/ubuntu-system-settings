#ifndef MIRDISPLAYS_H
#define MIRDISPLAYS_H

#include <mir_toolkit/mir_client_library.h>
#include <QObject>

class MirDisplays : public QObject
{
    Q_OBJECT
public:
    explicit MirDisplays(QObject *parent = 0) : QObject(parent) {}
    virtual ~MirDisplays();
    virtual MirDisplayConfiguration * getConfiguration() const = 0;
    virtual void setConfiguration(MirDisplayConfiguration * conf) = 0;
    virtual bool applyConfiguration(MirDisplayConfiguration * conf) = 0;
    virtual bool isConnected() = 0;

Q_SIGNALS:
    void configurationChanged() const;
};

#endif // MIRDISPLAYS_H
