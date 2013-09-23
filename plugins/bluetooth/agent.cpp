/*
 * Copyright (C) 2013 Canonical Ltd
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

#include <cassert>

#include "agent.h"

namespace
{
  const char * const REJECTED_TYPE = "org.bluez.Error.Rejected";
  const char * const REJECTED_TEXT = "The request was rejected";
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
void
Agent :: Release ()
{
  qDebug() << __FILE__ << ':' << __LINE__ << "agent released";
  Q_EMIT (onPairingDone());
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
void
Agent :: RequestConfirmation (const QDBusObjectPath& objectPath, uint passkey)
{
  qDebug() << __FILE__ << __LINE__ << "got passkey" << passkey;

  QSharedPointer<Device> device = m_devices.getDeviceFromPath (objectPath.path());
  if (device)
    {
      const uint tag = m_tag++;

      setDelayedReply (true);
      assert (!m_delayedReplies.contains(tag));
      m_delayedReplies[tag] = message();
      
      QString passkeyStr = QString("%1").arg(passkey, 6, 10, QChar('0'));
      Q_EMIT (passkeyConfirmationNeeded (tag, device.data(), passkeyStr));
    }
  else // confirmation requested for an unknown device..?!
    {
      m_connection.send (message().createErrorReply (REJECTED_TYPE, REJECTED_TEXT));
    }
}

/**
 * Invoked by the user-facing code after it prompts the user to confirm/cancel
 * the passkey passed from an Agent::passkeyConfirmationNeeded signal.
 *
 * @param tag: the tag sent in the emitted Agent::passkeyConfirmationNeeded signal
 * @param confirmed: true if user confirmed the passkey, false if they canceled
 */
void
Agent :: confirmPasskey (uint tag, bool confirmed)
{
  if (m_delayedReplies.contains(tag))
    {
      QDBusMessage message = m_delayedReplies[tag];

      if (confirmed)
        m_connection.send (message.createReply());
      else
        m_connection.send (message.createErrorReply (REJECTED_TYPE, REJECTED_TEXT));

      m_delayedReplies.remove (tag);
    }
}

/***
****
***/

/**
 * This method gets called when the service daemon
 * needs to get the passkey for an authentication.
 *
 * The return value should be a numeric value between 0-999999.
 *
 * Possible errors: org.bluez.Error.Rejected
 *                  org.bluez.Error.Canceled
 */
unsigned int
Agent :: RequestPasskey (const QDBusObjectPath& objectPath)
{
  QSharedPointer<Device> device = m_devices.getDeviceFromPath (objectPath.path());
  if (device)
    {
      const uint tag = m_tag++;

      setDelayedReply (true);
      assert (!m_delayedReplies.contains(tag));
      m_delayedReplies[tag] = message ();

      Q_EMIT (passkeyNeeded (tag, device.data()));
    }
  else // passkey requested for an unknown device..?!
    {
      m_connection.send (message().createErrorReply (REJECTED_TYPE, REJECTED_TEXT));
    }

  return 0;
}

/**
 * Invoked by the user-facing code after it prompts the user for a passkey
 * as a result of an Agent::passkeyNeeded signal.
 *
 * @param tag: the tag sent in the emitted Agent::passkeyNeeded signal
 * @param provided: true if user provided the passkey, false if they canceled
 * @param passkey: the passkey. Only valid if provided is true.
 */
void
Agent :: providePasskey (uint tag, bool provided, uint passkey)
{
  qDebug() << __FILE__ << __LINE__ << "provided" << provided << "passkey" << passkey;

  if (m_delayedReplies.contains(tag))
    {
      if (provided)
        m_connection.send (m_delayedReplies[tag].createReply (passkey));
      else
        m_connection.send (m_delayedReplies[tag].createErrorReply (REJECTED_TYPE, REJECTED_TEXT));

      m_delayedReplies.remove (tag);
    }
}

/***
****
***/

void
Agent :: DisplayPasskey (const QDBusObjectPath& objectPath, uint passkey, uchar entered)
{
  Q_UNUSED (objectPath);
  Q_UNUSED (passkey);
  Q_UNUSED (entered);

  // unimplemented -- unneeded for headsets
}

void
Agent :: Cancel ()
{
  // unimplemented -- companion function for DisplayPasskey
}

QString
Agent :: RequestPinCode (const QDBusObjectPath& device)
{
  // TODO: I'm not able to trigger this with any bluetooth devices and
  // Agent capabilities -- Everything triggers ConfirmPasskey or RequestPasskey.
  // Instead of creating untestable code here, throw an error

  Q_UNUSED (device);
  m_connection.send (message().createErrorReply (REJECTED_TYPE, REJECTED_TEXT));
  return "";
}
