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

#ifndef CLICK_SSO_H
#define CLICK_SSO_H

#include "sessiontoken.h"

#include <QObject>
#include <QString>
#include <QDebug>

namespace UpdatePlugin
{
namespace Click
{
class SSO : public QObject
{
    Q_OBJECT
public:
    explicit SSO(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~SSO() {};

    // Requests credentials.
    virtual void requestCredentials() = 0;

    // Invalidates credentials.
    virtual void invalidateCredentials() = 0;

Q_SIGNALS:
    // This signal is emitted when credentials are found..
    void credentialsFound(const SessionToken &token);

    // This signal is emitted when credentials are not found.
    void credentialsNotFound();

    // This signal is emitted when credentials are deleted.
    void credentialsDeleted();
};
} // Click
} // UpdatePlugin

#endif // CLICK_SSO_H
