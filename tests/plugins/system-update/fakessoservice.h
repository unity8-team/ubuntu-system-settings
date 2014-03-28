#ifndef FAKESSOSERVICE_H
#define FAKESSOSERVICE_H

#include <QObject>
#include <token.h>

using namespace UbuntuOne;

namespace UpdatePlugin {

class FakeSsoService : public QObject
{
    Q_OBJECT
public:
    explicit FakeSsoService(QObject *parent = 0);

    void getCredentials();
    
signals:
    void credentialsFound(const Token&);
    void credentialsNotFound();

};

}

#endif // FAKESSOSERVICE_H
