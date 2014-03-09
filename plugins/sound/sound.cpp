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
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#include <QDir>
#include "sound.h"
#include <unistd.h>

#define AS_INTERFACE "com.ubuntu.touch.AccountsService.Sound"

Sound::Sound(QObject *parent) :
    QObject(parent)
{
    connect (&m_accountsService,
             SIGNAL (propertyChanged (QString, QString)),
             this,
             SLOT (slotChanged (QString, QString)));

    connect (&m_accountsService,
             SIGNAL (nameOwnerChanged()),
             this,
             SLOT (slotNameOwnerChanged()));
}

void Sound::slotChanged(QString interface,
                                  QString property)
{
    if (interface != AS_INTERFACE)
        return;

    if (property == "SilentMode") {
        Q_EMIT silentModeChanged();
    } else if (property == "IncomingCallSound") {
        Q_EMIT incomingCallSoundChanged();
    } else if (property == "IncomingMessageSound") {
        Q_EMIT incomingMessageSoundChanged();
    }
}

void Sound::slotNameOwnerChanged()
{
    // Tell QML so that it refreshes its view of the property
    Q_EMIT incomingCallSoundChanged();
    Q_EMIT incomingMessageSoundChanged();
    Q_EMIT silentModeChanged();
}

QString Sound::getIncomingCallSound()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "IncomingCallSound").toString();
}

void Sound::setIncomingCallSound(QString sound)
{
    if (sound == getIncomingCallSound())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "IncomingCallSound",
                                      QVariant::fromValue(sound));
    Q_EMIT(incomingCallSoundChanged());
}

QString Sound::getIncomingMessageSound()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "IncomingMessageSound").toString();
}

void Sound::setIncomingMessageSound(QString sound)
{
    if (sound == getIncomingMessageSound())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "IncomingMessageSound",
                                      QVariant::fromValue(sound));
    Q_EMIT(incomingMessageSoundChanged());
}

bool Sound::getSilentMode()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "SilentMode").toBool();
}

void Sound::setSilentMode(bool enabled)
{
    if (enabled == getSilentMode())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "SilentMode",
                                      QVariant::fromValue(enabled));
    Q_EMIT(silentModeChanged());
}

QStringList Sound::listSounds(const QString &dirString)
{
    if (m_soundsList.isEmpty())
    {
        QDir soundsDir(dirString);
        soundsDir.setFilter(QDir::Files | QDir::NoSymLinks);
        m_soundsList = soundsDir.entryList();
    }
    return m_soundsList;
}
