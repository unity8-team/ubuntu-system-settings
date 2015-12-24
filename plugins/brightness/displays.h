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
 * A Displays manager class. Currently only suitable for Mir.
 * Note: It is assumed that the model will hold QScreen objects
 * some time in the future, and thus lose it's tight coupling
 * to Mir.
 */

#ifndef DISPLAYS_H
#define DISPLAYS_H

#include <QObject>
#include <QDebug>

#include "mirdisplays.h"
#include "displaymodel.h"

class Displays : public QObject
{
    Q_OBJECT
public:
    explicit Displays(QObject * parent = 0);
    ~Displays();
    QAbstractItemModel * displays();
    void applyDisplayConfiguration();

public Q_SLOTS:
    void updateAvailableMirDisplays();

private:
    DisplayListModel m_displaysModel;
    MirDisplays m_mirDisplays;
};

#endif // DISPLAYS_H
