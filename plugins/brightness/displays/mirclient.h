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

#ifndef MIRCLIENT_H
#define MIRCLIENT_H

#include <mir_toolkit/mir_client_library.h>
#include <QObject>

namespace DisplayPlugin
{
class MirClient : public QObject
{
    Q_OBJECT
public:
    explicit MirClient(QObject *parent = 0) : QObject(parent) {}
    virtual ~MirClient() {};
    virtual MirDisplayConfiguration* getConfiguration() const = 0;
    virtual void setConfiguration(MirDisplayConfiguration *conf) = 0;
    virtual bool applyConfiguration(MirDisplayConfiguration *conf) = 0;
    virtual bool isConnected() = 0;

Q_SIGNALS:
    void configurationChanged() const;
};
} // DisplayPlugin

#endif // MIRCLIENT_H
