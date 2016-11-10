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

#ifndef FAKEMIRCLIENT_H
#define FAKEMIRCLIENT_H

#include "mirclient.h"

#include <mir_toolkit/mir_client_library.h>
#include <QObject>

class FakeMirClient : public DisplayPlugin::MirClient
{
    Q_OBJECT
public:
    explicit FakeMirClient(QObject *parent = 0)
        : DisplayPlugin::MirClient(parent) {}
    virtual ~FakeMirClient() {}
    virtual MirDisplayConfiguration* getConfiguration() const override
    {
        return conf;
    }
    virtual void setConfiguration(MirDisplayConfiguration *conf) override
    {
        Q_UNUSED(conf);
    }
    virtual bool applyConfiguration(MirDisplayConfiguration *conf) override
    {
        // This usually happens via a mir callback. Fake it here.
        this->conf = conf;
        Q_EMIT configurationChanged();
        return true;
    }
    virtual bool isConnected() override
    {
        return connected;
    }

    bool connected = false;
    MirDisplayConfiguration *conf = nullptr;
};

#endif // FAKEMIRCLIENT_H
