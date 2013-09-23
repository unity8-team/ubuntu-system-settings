/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Charles Kerr <charles.kerr@canonical.com>
 */

#ifndef KILLSWITCH_H
#define KILLSWITCH_H

#include <QObject>

/**
 * Monitors whether or not bluetooth is blocked,
 * either by software (e.g., a session configuration setting)
 * or by hardware (e.g., user disabled it via a physical switch on her laptop).
 */
class KillSwitch : public QObject
{
    Q_OBJECT

public:
    bool isBlocked() const { return m_blocked; }

    /* Try to block/unblock bluetooth.
       This can fail if the requested state is overruled by a hardware block. */
    virtual void trySetBlocked(bool b) = 0;

Q_SIGNALS:
    void blockedChanged(bool blocked);

protected:
    void setBlocked(bool b) {
        if (m_blocked != b) {
            m_blocked = b;
            Q_EMIT(blockedChanged(b));
        }
    }

private:
    bool m_blocked = false;
};


class RfKillSwitchPrivate;

/**
 * KillSwitch impementation for Linux using /dev/rfkill
 */
class RfKillSwitch : public KillSwitch
{
    Q_OBJECT

public:
    RfKillSwitch();
    virtual ~RfKillSwitch();
    void trySetBlocked(bool b);

private:
    RfKillSwitchPrivate * d_ptr;
    Q_DECLARE_PRIVATE(RfKillSwitch);
    Q_PRIVATE_SLOT(d_func(), void onFileChanged());
};


#endif // KILLSWITCH_H
