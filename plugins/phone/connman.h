/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Ken VanDine <ken.vandine@canonical.com>
 *
*/

#ifndef CONNMAN_H
#define CONNMAN_H

#include <ofonoconnman.h>
#include <QDebug>
#include <QtCore>
#include <QObject>

class ConnMan : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool roamingAllowed READ roamingAllowed WRITE setRoamingAllowed NOTIFY roamingAllowedChanged)

public:
    ConnMan();
    bool roamingAllowed() const;
    void setRoamingAllowed(const bool &st);

signals:
    void roamingAllowedChanged ();

private slots:
    void onRoamingAllowedChanged(bool st);

private:
    OfonoConnMan *m;
    bool m_roam;
};


#endif // CONNMAN_H
