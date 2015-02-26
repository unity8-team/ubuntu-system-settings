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
    void setDeletedCredentials(bool value) { m_deletedCredentials = value; }
    
signals:
    void credentialsFound(const Token&);
    void credentialsNotFound();
    void credentialsDeleted();

private:
    bool m_validCredentials;
    bool m_deletedCredentials;
};

}

#endif // FAKESSOSERVICE_H
