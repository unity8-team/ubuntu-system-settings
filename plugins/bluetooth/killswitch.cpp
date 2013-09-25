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

#include <cerrno>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // write()

#include <QtDebug> // qWarning
#include <QFileSystemWatcher>
#include <QtGlobal>

#include <linux/rfkill.h>

#include "killswitch.h"

/***
****
***/

class RfKillSwitchPrivate
{
    Q_DECLARE_PUBLIC(RfKillSwitch)

    struct Entry
    {
        quint32 idx = 0;
        quint8 type = 0;
        quint8 soft = 0;
        quint8 hard = 0;
        Entry() {}
        Entry(const struct rfkill_event& e): idx(e.idx), type(e.type), soft(e.soft), hard(e.hard) {}
    };

    const char * const path = "/dev/rfkill";
    int fd = -1;
    bool blocked = false;
    QFileSystemWatcher watcher;
    mutable RfKillSwitch *q_ptr;
    std::map<quint32,Entry> entries;

    RfKillSwitchPrivate(RfKillSwitch * q):
        q_ptr(q)
    {
        fd = open(path, O_RDWR|O_NONBLOCK);
        if (fd == -1) {
            qWarning() <<  "Can't open" << path << " for use as a killswitch backend:" << strerror(errno);
        } else { // read everything that's already there, then watch for more
            readPendingEvents();
            watcher.addPath(path);
            QObject::connect(&watcher, SIGNAL(fileChanged(const QString&)),
                             q, SLOT(onFileChanged()));
        }
    }

    ~RfKillSwitchPrivate()
    {
        close(fd);
    }

    void onFileChanged()
    {
        readPendingEvents();
    }

    void readPendingEvents()
    {
        Q_Q(RfKillSwitch);

        int n = 0;
        while ((readEvent()))
            n++;

        if (n > 0) {
            // update our "blocked" property.
            // it should be true if any bluetooth entry is hard- or soft-blocked
            bool b = false;
            for (auto e : entries)
                if ((b = e.second.soft || e.second.hard))
                    break;
            q->setBlocked(b);
        }
    }

    bool readEvent()
    {
        struct rfkill_event event;
        const size_t n = sizeof(event);
        ssize_t n_read = read(fd, &event, n);

        if (n_read == n) {
            processEvent(event);
            return true;
        }

        return false;
    }

    void processEvent(const struct rfkill_event &event)
    {
        // we only want things that affect bluetooth
        if ((event.type != RFKILL_TYPE_ALL) &&
            (event.type != RFKILL_TYPE_BLUETOOTH))
          return;

        // update our entries lookup
        switch (event.op) {
        case RFKILL_OP_ADD:
        case RFKILL_OP_CHANGE:
        case RFKILL_OP_CHANGE_ALL:
            entries[event.idx] = Entry(event);
            break;

        case RFKILL_OP_DEL:
            entries.erase (event.idx);
            break;
        }
    }
};

/***
****
***/

RfKillSwitch::RfKillSwitch():
    d_ptr (new RfKillSwitchPrivate(this))
{
}

RfKillSwitch::~RfKillSwitch()
{
    delete d_ptr;
}

void RfKillSwitch::trySetBlocked(bool b)
{
    Q_D (RfKillSwitch);

    struct rfkill_event event;
    event.idx = 0;
    event.op = RFKILL_OP_CHANGE_ALL;
    event.type = RFKILL_TYPE_BLUETOOTH;
    event.soft = b;
    event.hard = false;

    const size_t n = sizeof (event);
    ssize_t n_written = write (d->fd, &event, n);
    if (n_written != n)
        qWarning() << "Couldn't write rfkill event:" << strerror(errno);
}

#include "moc_killswitch.cpp"
