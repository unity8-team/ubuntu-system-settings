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

#include "sso.h"

#include <token.h>

class MockSSO : public UpdatePlugin::Click::SSO
{
public:
    MockSSO(QObject *parent = 0) : UpdatePlugin::Click::SSO(parent) {};
    virtual ~MockSSO() {};

    virtual void requestCredentials() override
    {
        requestCredentialsCalled = true;
    }

    virtual void invalidateCredentials() override
    {
        invalidateCredentialsCalled = true;
    }

    void mockCredentialsRequestSucceeded(const UbuntuOne::Token &token)
    {
        Q_EMIT credentialsRequestSucceeded(token);
    }
    void mockCredentialsRequestFailed()
    {
        Q_EMIT credentialsRequestFailed();
    }

    bool requestCredentialsCalled = false;
    bool invalidateCredentialsCalled = false;
signals:
};

#endif // MOCK_CLICK_SSO_H
