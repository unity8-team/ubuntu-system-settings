/*
 * Copyright (C) 2013-2015 Canonical Ltd
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
 * Charles Kerr <charles.kerr@canonical.com>
 *
 */

#include "agent.h"

#include <cassert>

/***
****
***/

void Agent::cancel(QDBusMessage msg, const char *functionName)
{
  QString name = "org.bluez.Error.Canceled";
  QString text = QString("The request was canceled: %1").arg(functionName);
  m_connection.send(msg.createErrorReply(name, text));
}

void Agent::reject(QDBusMessage msg, const char *functionName)
{
  QString name = "org.bluez.Error.Rejected";
  QString text = QString("The request was rejected: %1").arg(functionName);
  m_connection.send(msg.createErrorReply(name, text));
}

QSharedPointer<Device> Agent::findOrCreateDevice(const QDBusObjectPath &path)
{
    auto device = m_devices.getDeviceFromPath(path.path());

    // If the device doesn't exist we just couldn't add it to our
    // internall list as we didn't received the corresponding dbus
    // signal for that yet. This normally happens when a remote device
    // wants to pair with us but we didn't discovered that device yet.
    // We simply create an entry for this new device then and will
    // continue as normal.
    if (!device)
        device = m_devices.addDeviceFromPath(path);

    return device;
}

/***
****
***/

/**
 * This method gets called when the service daemon
 * unregisters the agent. An agent can use it to do
 * cleanup tasks. There is no need to unregister the
 * agent, because when this method gets called it has
 * already been unregistered.
 */
void Agent::Release()
{
    Q_EMIT(releaseNeeded());
}

/***
****
***/

/**
 * This method gets called when the service daemon
 * needs to confirm a passkey for an authentication.
 *
 * To confirm the value it should return an empty reply
 * or an error in case the passkey is invalid.
 *
 * Note that the passkey will always be a 6-digit number,
 * so the display should be zero-padded at the start if
 * the value contains less than 6 digits.
 *
 * Possible errors: org.bluez.Error.Rejected
 *                  org.bluez.Error.Canceled
 */
void Agent::RequestConfirmation(const QDBusObjectPath &objectPath, uint passkey)
{
    if (auto device = findOrCreateDevice(objectPath)) {
        const uint tag = m_tag++;

        setDelayedReply(true);
        assert(!m_delayedReplies.contains(tag));
        m_delayedReplies[tag] = message();

        QString passkeyStr = QString("%1").arg(passkey, 6, 10, QChar('0'));
        Q_EMIT(passkeyConfirmationNeeded(tag, device.data(), passkeyStr));
    } else { // confirmation requested for an unknown device..?!
        reject(message(), __func__);
    }
}

/**
 * Invoked by the user-facing code after it prompts the user to confirm/cancel
 * the passkey passed from an Agent::passkeyConfirmationNeeded signal.
 *
 * @param tag: the tag from the Agent::passkeyConfirmationNeeded signal
 * @param confirmed: true if user confirmed the passkey, false if they canceled
 */
void Agent::confirmPasskey(uint tag, bool confirmed)
{
    if (m_delayedReplies.contains(tag)) {
        QDBusMessage message = m_delayedReplies[tag];

        if (confirmed)
            m_connection.send(message.createReply());
        else
            cancel(message, __func__);

        m_delayedReplies.remove(tag);
    }
}

QString Agent::RequestPinCode(const QDBusObjectPath &objectPath)
{
    if (auto device = findOrCreateDevice(objectPath)) {
        const uint tag = m_tag++;

        setDelayedReply(true);
        assert(!m_delayedReplies.contains(tag));
        m_delayedReplies[tag] = message();

        Q_EMIT(pinCodeNeeded(tag, device.data()));

    } else { // passkey requested for an unknown device..?!
        reject(message(), __func__);
    }

  return 0;
}

/**
 * This method gets called when the service daemon
 * needs to get the passkey for an authentication.
 *
 * The return value should be a numeric value between 0-999999.
 *
 * Possible errors: org.bluez.Error.Rejected
 *                  org.bluez.Error.Canceled
 */
unsigned int Agent::RequestPasskey(const QDBusObjectPath &objectPath)
{
    if (auto device = findOrCreateDevice(objectPath)) {
        const uint tag = m_tag++;

        setDelayedReply(true);
        assert(!m_delayedReplies.contains(tag));
        m_delayedReplies[tag] = message();

        Q_EMIT(passkeyNeeded(tag, device.data()));

    } else { // passkey requested for an unknown device..?!
        reject(message(), __func__);
    }

  return 0;
}

/**
 * Invoked by the user-facing code after it prompts the user for a passkey
 * as a result of an Agent::passkeyNeeded signal.
 *
 * @param tag: the tag from the Agent::passkeyNeeded signal
 * @param provided: true if user provided the passkey, false if they canceled
 * @param passkey: the passkey. Only valid if provided is true.
 */
void Agent::providePasskey(uint tag, bool provided, uint passkey)
{
    if (m_delayedReplies.contains(tag)) {
        if (provided)
            m_connection.send(m_delayedReplies[tag].createReply(passkey));
        else
            cancel(m_delayedReplies[tag], __func__);

        m_delayedReplies.remove(tag);
    }
}

/***
****
***/

/**
 * Invoked by the user-facing code after it prompts the user for a PIN code
 * from an Agent::pinCodeNeeded() signal.
 *
 * @param tag: the tag from the Agent::passkeyConfirmationNeeded signal
 * @param confirmed: true if user confirmed the passkey, false if they canceled
 */
void Agent::providePinCode(uint tag, bool confirmed, QString pinCode)
{
    if (m_delayedReplies.contains(tag)) {
        QDBusMessage message = m_delayedReplies[tag];

        if (confirmed)
            m_connection.send(message.createReply(qVariantFromValue(pinCode)));
        else
            cancel(message, __func__);

        m_delayedReplies.remove(tag);
    }
}

void Agent::DisplayPinCode(const QDBusObjectPath &objectPath, QString pincode)
{
    if (auto device = findOrCreateDevice(objectPath)) {
        Q_EMIT(displayPinCodeNeeded(device.data(), pincode));
    } else {
        reject(message(), __func__);
    }
}

void Agent::DisplayPasskey(const QDBusObjectPath &objectPath, uint passkey, ushort entered)
{
    if (auto device = findOrCreateDevice(objectPath)) {
        QString passkeyStr = QString("%1").arg(passkey, 6, 10, QChar('0'));
        Q_EMIT(displayPasskeyNeeded(device.data(), passkeyStr, entered));
    } else {
        reject(message(), __func__);
    }
}

/**
 * This method gets called to indicate that the agent
 * request failed before a reply was returned.
 */
void Agent::Cancel()
{
    qWarning() << "Cancel callback called";

    Q_EMIT(cancelNeeded());
}

void Agent::RequestAuthorization(const QDBusObjectPath &objectPath)
{
    qWarning() << "Authorization requested for device"
               << objectPath.path();

    if (auto device = findOrCreateDevice(objectPath)) {
        const uint tag = m_tag++;

        setDelayedReply(true);
        assert(!m_delayedReplies.contains(tag));
        m_delayedReplies[tag] = message();

        Q_EMIT(authorizationRequested(tag, device.data()));
    }
    else {
        reject(message(), __func__);
    }
}

void Agent::authorizationRequestCallback(uint tag, bool allow)
{
    if (m_delayedReplies.contains(tag)) {
        QDBusMessage message = m_delayedReplies[tag];

        if (allow)
            m_connection.send(message.createReply());
        else
            reject(message, __func__);

        m_delayedReplies.remove(tag);
    }
}

void Agent::displayPinCodeCallback(uint tag)
{
    if (m_delayedReplies.contains(tag)) {
        QDBusMessage message = m_delayedReplies[tag];

        cancel(message, __func__);

        m_delayedReplies.remove(tag);
    }
}

/**
 * Invoked by the user-facing code after it prompts the user to cancel
 * the passkey passed from an Agent::displayPasskeyNeeded signal.
 *
 * @param tag: the tag from the Agent::displayPasskeyNeeded signal
 */
void Agent::displayPasskeyCallback(uint tag)
{
    if (m_delayedReplies.contains(tag)) {
        QDBusMessage message = m_delayedReplies[tag];

        cancel(message, __func__);

        m_delayedReplies.remove(tag);
    }
}
