/*
 * Copyright (C) 2015 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
 */

#ifndef MIRDISPLAYS_H
#define MIRDISPLAYS_H

#include <QObject>
#include <QDebug>
#include <mir_toolkit/mir_client_library.h>

class MirDisplays : public QObject
{
    Q_OBJECT
public:
    explicit MirDisplays(QObject *parent = 0);
    ~MirDisplays();
    MirDisplayConfiguration * getConfiguration() const;
    void setConfiguration(MirDisplayConfiguration * conf);
    void applyConfiguration(MirDisplayConfiguration * conf);
    bool isConnected();

Q_SIGNALS:
    void configurationChanged() const;

private:
    bool connect();
    MirConnection * m_mir_connection;
    MirDisplayConfiguration * m_configuration;
};

#endif // MIRDISPLAYS_H
