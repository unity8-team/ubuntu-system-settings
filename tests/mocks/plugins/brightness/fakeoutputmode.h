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

#ifndef FAKE_OUTPUTMODE_H
#define FAKE_OUTPUTMODE_H

#include "outputmode/outputmode.h"

namespace DisplayPlugin
{
class FakeOutputMode : public OutputMode
{
public:
    virtual ~FakeOutputMode()
    {
    }

    virtual int getWidth() override
    {
        return m_width;
    }
    virtual int getHeight() override
    {
        return m_height;
    }
    virtual double getRefreshRate() override
    {
        return m_refreshRate;
    }
    virtual QString toString() override
    {
        return m_string;
    }

    int m_width = 0;
    int m_height = 0;
    double m_refreshRate = 0;
    QString m_string = QString::null;
};

} // DisplayPlugin

#endif // FAKE_OUTPUTMODE_H
