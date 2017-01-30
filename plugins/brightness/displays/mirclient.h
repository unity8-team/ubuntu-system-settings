/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
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

#include "output/output.h"

#include <mir_toolkit/mir_client_library.h>
#include <QList>
#include <QObject>
#include <QSharedPointer>

namespace DisplayPlugin
{
class Q_DECL_EXPORT MirClient : public QObject
{
    Q_OBJECT
public:
    explicit MirClient(QObject *parent = 0) : QObject(parent) {}
    virtual ~MirClient() {};
    virtual MirDisplayConfig* getConfiguration() const = 0;
    virtual void setConfiguration(MirDisplayConfig *conf) = 0;
    virtual void applyConfiguration(MirDisplayConfig *conf) = 0;
    virtual bool isConnected() = 0;
    virtual QList<QSharedPointer<Output>> outputs() = 0;

Q_SIGNALS:
    void configurationChanged();
    void configurationFailed(const QString &errorMessage);
};
} // DisplayPlugin

#endif // MIRCLIENT_H
