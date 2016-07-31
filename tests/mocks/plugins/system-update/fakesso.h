/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MOCK_CLICK_SSO_H
#define MOCK_CLICK_SSO_H

#include "click/sessiontoken.h"
#include "click/sso.h"

namespace UpdatePlugin
{
class MockSSO : public Click::SSO
{
public:
    MockSSO(QObject *parent = nullptr) : Click::SSO(parent) {};
    virtual ~MockSSO() {};

    virtual void requestCredentials() override
    {
        requestCredentialsCalled = true;
    }

    virtual void invalidateCredentials() override
    {
        invalidateCredentialsCalled = true;
    }

    void mockCredentialsFound(Click::SessionToken *token)
    {
        Q_EMIT credentialsFound(token);
    }

    void mockCredentialsNotFound()
    {
        Q_EMIT credentialsNotFound();
    }

    void mockCredentialsDeleted()
    {
        Q_EMIT credentialsDeleted();
    }

    bool requestCredentialsCalled = false;
    bool invalidateCredentialsCalled = false;
};
} // UpdatePlugin
#endif // MOCK_CLICK_SSO_H
