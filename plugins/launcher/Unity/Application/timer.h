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

#ifndef QTMIR_TIMER_H
#define QTMIR_TIMER_H

#include "timesource.h"

#include <QObject>
#include <QPointer>
#include <QTimer>

namespace qtmir {

/* Defines an interface for a Timer. Useful for tests. */
class AbstractTimer : public QObject
{
    Q_OBJECT
public:
    AbstractTimer(QObject *parent) : QObject(parent), m_isRunning(false) {}
    virtual int interval() const = 0;
    virtual void setInterval(int msecs) = 0;
    virtual void start() { m_isRunning = true; }
    virtual void stop() { m_isRunning = false; }
    bool isRunning() const { return m_isRunning; }
    virtual bool isSingleShot() const = 0;
    virtual void setSingleShot(bool value) = 0;
Q_SIGNALS:
    void timeout();
private:
    bool m_isRunning;
};

/* Essentially a QTimer wrapper */
class Timer : public AbstractTimer
{
    Q_OBJECT
public:
    Timer(QObject *parent = nullptr);

    int interval() const override;
    void setInterval(int msecs) override;
    void start() override;
    void stop() override;
    bool isSingleShot() const override;
    void setSingleShot(bool value) override;
private:
    QTimer m_timer;
};

/* For tests */
class FakeTimer : public AbstractTimer
{
    Q_OBJECT
public:
    FakeTimer(const SharedTimeSource &timeSource, QObject *parent = nullptr);

    void update();
    qint64 nextTimeoutTime() const { return m_nextTimeoutTime; }

    int interval() const override;
    void setInterval(int msecs) override;
    void start() override;
    bool isSingleShot() const override;
    void setSingleShot(bool value) override;
private:
    int m_interval;
    bool m_singleShot;
    SharedTimeSource m_timeSource;
    qint64 m_nextTimeoutTime;
};

} // namespace qtmir

#endif // QTMIR_TIMER_H
