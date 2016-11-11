/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "timer.h"

using namespace qtmir;

Timer::Timer(QObject *parent) : AbstractTimer(parent)
{
    m_timer.setSingleShot(false);
    connect(&m_timer, &QTimer::timeout, this, &AbstractTimer::timeout);
}

int Timer::interval() const
{
    return m_timer.interval();
}

void Timer::setInterval(int msecs)
{
    m_timer.setInterval(msecs);
}

void Timer::start()
{
    m_timer.start();
    AbstractTimer::start();
}

void Timer::stop()
{
    m_timer.stop();
    AbstractTimer::stop();
}

bool Timer::isSingleShot() const
{
    return m_timer.isSingleShot();
}

void Timer::setSingleShot(bool value)
{
    m_timer.setSingleShot(value);
}

/////////////////////////////////// FakeTimer //////////////////////////////////

FakeTimer::FakeTimer(const SharedTimeSource &timeSource, QObject *parent)
    : qtmir::AbstractTimer(parent)
    , m_interval(0)
    , m_singleShot(false)
    , m_timeSource(timeSource)
{
}

void FakeTimer::update()
{
    if (!isRunning()) {
        return;
    }

    if (m_nextTimeoutTime <= m_timeSource->msecsSinceReference()) {
        if (isSingleShot()) {
            stop();
        } else {
            m_nextTimeoutTime += interval();
        }
        Q_EMIT timeout();
    }
}

void FakeTimer::start()
{
    AbstractTimer::start();
    m_nextTimeoutTime = m_timeSource->msecsSinceReference() + (qint64)interval();
}

int FakeTimer::interval() const
{
    return m_interval;
}

void FakeTimer::setInterval(int msecs)
{
    m_interval = msecs;
}

bool FakeTimer::isSingleShot() const
{
    return m_singleShot;
}

void FakeTimer::setSingleShot(bool value)
{
    m_singleShot = value;
}
