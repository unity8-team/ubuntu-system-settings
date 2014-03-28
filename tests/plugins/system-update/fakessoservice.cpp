#include "fakessoservice.h"

namespace UpdatePlugin {

FakeSsoService::FakeSsoService(QObject *parent) :
    QObject(parent)
{
}

void FakeSsoService::getCredentials()
{
    Token token("token_key", "token_secret", "consumer_key", "consumer_secret");
    emit this->credentialsFound(token);
}

}
