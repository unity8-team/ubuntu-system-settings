/*
 * Copyright (C) 2016 Canonical Ltd
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
 */

#ifndef WIFI_H
#define WIFI_H

#include <QObject>

class Wifi : public QObject
{
    Q_OBJECT
    Q_PROPERTY (bool wapiSupported
                READ wapiSupported
                CONSTANT)
public:
    explicit Wifi(QObject *parent = 0);
    bool wapiSupported() const;
};

#endif // WIFI_H
