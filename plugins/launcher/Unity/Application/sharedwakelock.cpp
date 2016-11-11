/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sharedwakelock.h"
#include "abstractdbusservicemonitor.h"
#include "logging.h"

#include <QDBusAbstractInterface>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QFile>

namespace qtmir {

const int POWERD_SYS_STATE_ACTIVE = 1; // copied from private header file powerd.h
const char cookieFile[] = "/tmp/qtmir_powerd_cookie";

/**
 * @brief The Wakelock class - wraps a single system wakelock
 * Should the PowerD service vanish from the bus, the wakelock will be re-acquired when it re-joins the bus.
 */
class Wakelock : public AbstractDBusServiceMonitor
{
    Q_OBJECT
public:
    Wakelock(const QDBusConnection &connection) noexcept
        : AbstractDBusServiceMonitor(QStringLiteral("com.canonical.powerd"), QStringLiteral("/com/canonical/powerd"), QStringLiteral("com.canonical.powerd"), connection)
        , m_wakelockEnabled(false)
    {
        // (re-)acquire wake lock when powerd (re-)appears on the bus
        QObject::connect(this, &Wakelock::serviceAvailableChanged,
                         this, &Wakelock::onServiceAvailableChanged);

        // WORKAROUND: if shell crashed while it held a wakelock, due to bug lp:1409722 powerd will not have released
        // the wakelock for it. As workaround, we save the cookie to file and restore it if possible.
        QFile cookieCache(cookieFile);
        if (cookieCache.exists() && cookieCache.open(QFile::ReadOnly | QFile::Text)) {
            m_wakelockEnabled = true;
            m_cookie = cookieCache.readAll();
        }
    }

    virtual ~Wakelock() noexcept
    {
        release();
    }

    Q_SIGNAL void enabledChanged(bool);
    bool enabled() const
    {
        return m_wakelockEnabled;
    }

    void acquire()
    {
        if (m_wakelockEnabled) { // wakelock already requested/set
            return;
        }
        m_wakelockEnabled = true;

        acquireWakelock();
    }

    void release()
    {
        QFile::remove(cookieFile);

        if (!m_wakelockEnabled) { // no wakelock already requested/set
            return;
        }
        m_wakelockEnabled = false;
        Q_EMIT enabledChanged(false);

        if (!serviceAvailable()) {
            qWarning() << "com.canonical.powerd DBus interface not available, presuming no wakelocks held";
            return;
        }

        if (!m_cookie.isEmpty()) {
            dbusInterface()->asyncCall(QStringLiteral("clearSysState"), QString(m_cookie));
            qCDebug(QTMIR_SESSIONS) << "Wakelock released" << m_cookie;
            m_cookie.clear();
        }
    }

private Q_SLOTS:
    void onServiceAvailableChanged(bool available)
    {
        // Assumption is if service vanishes & reappears on the bus, it has lost its wakelock state and
        // we must re-acquire if necessary
        if (!m_wakelockEnabled) {
            return;
        }

        if (available) {
            acquireWakelock();
        } else {
            m_cookie.clear();
            QFile::remove(cookieFile);
        }
    }

    void onWakeLockAcquired(QDBusPendingCallWatcher *call)
    {
        QDBusPendingReply<QString> reply = *call;
        if (reply.isError()) {
            qCDebug(QTMIR_SESSIONS) << "Wakelock was NOT acquired, error:"
                                    << QDBusError::errorString(reply.error().type());
            if (m_wakelockEnabled) {
                m_wakelockEnabled = false;
                Q_EMIT enabledChanged(false);
            }

            call->deleteLater();
            return;
        }
        QByteArray cookie = reply.argumentAt<0>().toLatin1();
        call->deleteLater();

        if (!m_wakelockEnabled || !m_cookie.isEmpty()) {
            // notified wakelock was created, but we either don't want it, or already have one - release it immediately
            dbusInterface()->asyncCall(QStringLiteral("clearSysState"), QString(cookie));
            return;
        }

        m_cookie = cookie;

        // see WORKAROUND above for why we save cookie to disk
        QFile cookieCache(cookieFile);
        cookieCache.open(QFile::WriteOnly | QFile::Text);
        cookieCache.write(m_cookie);

        qCDebug(QTMIR_SESSIONS) << "Wakelock acquired" << m_cookie;
        Q_EMIT enabledChanged(true);
    }

private:
    void acquireWakelock()
    {
        if (!serviceAvailable()) {
            qWarning() << "com.canonical.powerd DBus interface not available, waiting for it";
            return;
        }

        QDBusPendingCall pcall = dbusInterface()->asyncCall(QStringLiteral("requestSysState"), "active", POWERD_SYS_STATE_ACTIVE);

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
        QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                         this, &Wakelock::onWakeLockAcquired);
    }

    QByteArray m_cookie;
    bool m_wakelockEnabled;

    Q_DISABLE_COPY(Wakelock)
};

#include "sharedwakelock.moc"

/**
 * @brief SharedWakelock - allow a single wakelock instance to be shared between multiple owners
 *
 * QtMir has application management duties to perform even if display is off. To prevent device
 * going to deep sleep before QtMir is ready, have QtMir register a system wakelock when it needs to.
 *
 * This class allows multiple objects to own the wakelock simultaneously. The wakelock is first
 * registered when acquire has been called by one caller. Multiple callers may then share the
 * wakelock. The wakelock is only destroyed when all callers have called release.
 *
 * Note a caller cannot have multiple shares of the wakelock. Multiple calls to acquire are ignored.
 */

SharedWakelock::SharedWakelock(const QDBusConnection &connection)
    : m_wakelock(new Wakelock(connection))
{
    connect(m_wakelock.data(), &Wakelock::enabledChanged,
            this, &SharedWakelock::enabledChanged);
}

// Define empty deconstructor here, as QScopedPointer<Wakelock> requires the destructor of the Wakelock class
// to be defined first.
SharedWakelock::~SharedWakelock()
{
}

bool SharedWakelock::enabled() const
{
    return m_wakelock->enabled();
}

void SharedWakelock::acquire(const QObject *caller)
{
    if (caller == nullptr || m_owners.contains(caller)) {
        return;
    }

    // register a slot to remove itself from owners list if destroyed
    QObject::connect(caller, &QObject::destroyed, this, &SharedWakelock::release);

    m_wakelock->acquire();

    m_owners.insert(caller);
}

void SharedWakelock::release(const QObject *caller)
{
    if (caller == nullptr || !m_owners.remove(caller)) {
        return;
    }

    QObject::disconnect(caller, &QObject::destroyed, this, 0);

    if (m_owners.empty()) {
        m_wakelock->release();
    }
}

} // namespace qtmir
