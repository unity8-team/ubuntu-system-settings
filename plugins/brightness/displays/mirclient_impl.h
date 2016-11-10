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

#ifndef MIRCLIENT_IMPL_H
#define MIRCLIENT_IMPL_H

#include "mirclient.h"

namespace DisplayPlugin
{
class MirClientImpl : public MirClient
{
    Q_OBJECT
public:
    explicit MirClientImpl(QObject *parent = 0);
    ~MirClientImpl();
    virtual MirDisplayConfiguration* getConfiguration() const;
    virtual void setConfiguration(MirDisplayConfiguration *conf) override;
    virtual bool applyConfiguration(MirDisplayConfiguration *conf) override;
    virtual bool isConnected() override;

private:
    void connect();
    MirConnection *m_mir_connection;
    MirDisplayConfiguration *m_configuration;
};
} // DisplayPlugin

#endif // MIRCLIENT_IMPL_H
