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

#ifndef CLICK_SESSIONTOKEN_H
#define CLICK_SESSIONTOKEN_H

#include <QString>

namespace UpdatePlugin
{
namespace Click
{
// Interface for a session token.
class SessionToken
{
public:
    explicit SessionToken() {};
    virtual ~SessionToken() {};
    virtual bool isValid() const = 0;
    virtual QString signUrl(const QString url,
                            const QString method,
                            bool asQuery = false) const = 0;
};
} // Click
} // UpdatePlugin

#endif // CLICK_SESSIONTOKEN_H
