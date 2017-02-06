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

#ifndef DISPLAYS_OUTPUTMODE_H
#define DISPLAYS_OUTPUTMODE_H

#include <QtCore/QtGlobal>

namespace DisplayPlugin
{
// Represents an abstraction of an output mode.
class Q_DECL_EXPORT OutputMode
{
public:
    virtual ~OutputMode() {};

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual double getRefreshRate() = 0;
    virtual QString toString() = 0;
};
} // DisplayPlugin

#endif // DISPLAYS_OUTPUTMODE_H
