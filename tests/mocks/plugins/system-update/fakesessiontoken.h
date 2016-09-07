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

#ifndef MOCK_SESSIONTOKEN_H
#define MOCK_SESSIONTOKEN_H

#include "click/sessiontoken.h"

#include <QString>

namespace UpdatePlugin
{
class MockSessionToken : public Click::SessionToken
{
public:
    MockSessionToken() : Click::SessionToken() {};
    virtual ~MockSessionToken() {};

    virtual bool isValid() const
    {
        return valid;
    };

    virtual QString signUrl(const QString url,
                            const QString method,
                            bool asQuery = false) const
    {
        Q_UNUSED(url);
        Q_UNUSED(method);
        Q_UNUSED(asQuery);
        return signedUrl;
    };

    bool valid = true;
    QString signedUrl = QString::null;
};
} // UpdatePlugin
#endif // MOCK_SESSIONTOKEN_H
