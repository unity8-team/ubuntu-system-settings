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

#ifndef CLICK_SESSIONTOKEN_IMPL_H
#define CLICK_SESSIONTOKEN_IMPL_H

#include "sessiontoken.h"

#include <token.h>

namespace UpdatePlugin
{
namespace Click
{
class SessionTokenImpl : public SessionToken
{
public:
    // Creates an invalid SessionToken.
    explicit SessionTokenImpl();
    // Creates a SessionToken using a UbuntuOne Token.
    explicit SessionTokenImpl(const UbuntuOne::Token &token);
    virtual ~SessionTokenImpl() {};
    virtual bool isValid() const override;
    virtual QString signUrl(const QString url,
                            const QString method,
                            bool asQuery = false) const override;
private:
    UbuntuOne::Token m_token;
};
} // Click
} // UpdatePlugin

#endif // CLICK_SESSIONTOKEN_IMPL_H
