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
#include "MockDisplayModel.h"
#include "MockBrightness.h"

#include <QQmlEngine>

MockBrightness::MockBrightness(QObject *parent)
    : QObject(parent)
    , m_displays()
    , m_changedDisplays()
{
    m_changedDisplays.filterOnUncommittedChanges(true);
    m_changedDisplays.setSourceModel(&m_displays);
}

bool MockBrightness::getPowerdRunning() const
{
    return m_powerdRunning;
}

bool MockBrightness::getAutoBrightnessAvailable() const
{
    return m_autoBrightnessAvailable;
}

bool MockBrightness::getWidiSupported() const
{
    return m_widiSupported;
}

void MockBrightness::setPowerdRunning(const bool running)
{
    m_powerdRunning = running;
    Q_EMIT powerdRunningChanged();
}

void MockBrightness::setAutoBrightnessAvailable(const bool available)
{
    m_autoBrightnessAvailable = available;
    Q_EMIT autoBrightnessAvailableChanged();
}

void MockBrightness::setWidiSupported(const bool supported)
{
    m_widiSupported = supported;
    Q_EMIT widiSupportedChanged();
}

QAbstractItemModel* MockBrightness::allDisplays()
{
    auto ret = &m_displays;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel* MockBrightness::changedDisplays()
{
    auto ret = &m_changedDisplays;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

void MockBrightness::applyDisplayConfiguration()
{

}

MockDisplayModel* MockBrightness::displayModel()
{
    return (MockDisplayModel*) allDisplays();
}
