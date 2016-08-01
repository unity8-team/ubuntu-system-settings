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

#include "click/sessiontoken_impl.h"

#include <QDebug>
namespace UpdatePlugin
{
namespace Click
{
SessionTokenImpl::SessionTokenImpl() : SessionTokenImpl(UbuntuOne::Token())
{
}

SessionTokenImpl::SessionTokenImpl(const UbuntuOne::Token &token)
    : SessionToken()
    , m_token(token)
{
}

bool SessionTokenImpl::isValid() const
{
    return m_token.isValid();
}

QString SessionTokenImpl::signUrl(const QString url,
                                  const QString method,
                                  bool asQuery) const
{
    return m_token.signUrl(url, method, asQuery);
}
} // Click
} // UpdatePlugin
