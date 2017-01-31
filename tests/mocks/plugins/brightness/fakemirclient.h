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

#include "output/output.h"

#include <mir_toolkit/mir_client_library.h>
#include <QObject>

class FakeMirClient : public DisplayPlugin::MirClient
{
    Q_OBJECT
public:
    explicit FakeMirClient(QObject *parent = 0)
        : DisplayPlugin::MirClient(parent) {}
    virtual ~FakeMirClient() {}
    virtual MirDisplayConfig* getConfiguration() const override
    {
        return conf;
    }
    virtual void setConfiguration(MirDisplayConfig *conf) override
    {
        Q_UNUSED(conf);
    }
    virtual void applyConfiguration(MirDisplayConfig *conf) override
    {
        // This usually happens via a mir callback. Fake it here.
        this->conf = conf;
        Q_EMIT configurationChanged();
    }
    virtual bool isConnected() override
    {
        return connected;
    }
    virtual bool isConfigurationValid() override
    {
        return configurationValid;
    }
    virtual QList<QSharedPointer<DisplayPlugin::Output>> outputs() override
    {
        return m_outputs;
    }

    bool connected = false;
    bool configurationValid = false;
    MirDisplayConfig *conf = nullptr;
    QList<QSharedPointer<DisplayPlugin::Output>> m_outputs;
};

#endif // FAKEMIRCLIENT_H
