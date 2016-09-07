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

#ifndef CLICK_SSO_IMPL_H
#define CLICK_SSO_IMPL_H

#include "click/sso.h"

#include <token.h>
#include <ssoservice.h>

#include <QObject>
#include <QString>

/* Having the full namespaced name in a slot seems to confuse
Qt at runtime, so we need this declaration. */
using namespace UbuntuOne;

namespace UpdatePlugin
{
namespace Click
{
class SSOImpl : public SSO
{
    Q_OBJECT
public:
    explicit SSOImpl(QObject *parent = nullptr);
    virtual ~SSOImpl() {};
    virtual void requestCredentials() override;
    virtual void invalidateCredentials() override;
private Q_SLOTS:
    void handleCredentialsFound(const Token &token);
private:
    UbuntuOne::SSOService *m_service;
};
} // Click
} // UpdatePlugin

#endif // CLICK_SSO_IMPL_H
