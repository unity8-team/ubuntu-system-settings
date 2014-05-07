#include "fakessoservice.h"

namespace UpdatePlugin {

FakeSsoService::FakeSsoService(QObject *parent) :
    QObject(parent),
    m_validCredentials(true)
{
}

void FakeSsoService::getCredentials()
{
    if(m_validCredentials) {
        Token token("token_key", "token_secret", "consumer_key", "consumer_secret");
        emit this->credentialsFound(token);
    } else {
        emit this->credentialsNotFound();
    }
}

}
