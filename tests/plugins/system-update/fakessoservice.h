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

    void setValidCredentials(bool value) { m_validCredentials = value; }
    
signals:
    void credentialsFound(const Token&);
    void credentialsNotFound();

private:
    bool m_validCredentials;
};

}

#endif // FAKESSOSERVICE_H
