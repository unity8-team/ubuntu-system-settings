/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Antti Kaijanmäki <antti.kaijanmaki@canonical.com>
 *
 */

#include <QObject>
#include "urfkill-proxy.h"

#ifndef FLIGHT_MODE_HELPER_H
#define FLIGHT_MODE_HELPER_H

class FlightModeHelper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool inFlightMode
               READ inFlightMode
               NOTIFY inFlightModeChanged)

public:
    FlightModeHelper(QObject *parent = 0);
    ~FlightModeHelper();

    Q_INVOKABLE void setFlightMode(bool value);

    bool inFlightMode();

signals:
    void inFlightModeChanged();

private:
    org::freedesktop::URfkill *m_urfkill;
    bool m_isFlightMode;
};

#endif
