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

#include "sound.h"

#include <QDir>
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
    } else if (property == "IncomingCallVibrate") {
        Q_EMIT incomingCallVibrateChanged();
    } else if (property == "IncomingMessageVibrate") {
        Q_EMIT incomingMessageVibrateChanged();
    } else if (property == "IncomingCallVibrateSilentMode") {
        Q_EMIT incomingCallVibrateSilentModeChanged();
    } else if (property == "IncomingMessageVibrateSilentMode") {
        Q_EMIT incomingMessageVibrateSilentModeChanged();
    }
}

void Sound::slotNameOwnerChanged()
{
    // Tell QML so that it refreshes its view of the property
    Q_EMIT incomingCallSoundChanged();
    Q_EMIT incomingMessageSoundChanged();
    Q_EMIT silentModeChanged();
    Q_EMIT incomingCallVibrateChanged();
    Q_EMIT incomingMessageVibrateChanged();
    Q_EMIT incomingCallVibrateSilentModeChanged();
    Q_EMIT incomingMessageVibrateSilentModeChanged();
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

bool Sound::getIncomingCallVibrate()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "IncomingCallVibrate").toBool();
}

void Sound::setIncomingCallVibrate(bool enabled)
{
    if (enabled == getIncomingCallVibrate())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "IncomingCallVibrate",
                                      QVariant::fromValue(enabled));
    Q_EMIT(incomingCallVibrateChanged());
}

bool Sound::getIncomingCallVibrateSilentMode()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "IncomingCallVibrateSilentMode").toBool();
}

void Sound::setIncomingCallVibrateSilentMode(bool enabled)
{
    if (enabled == getIncomingCallVibrateSilentMode())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "IncomingCallVibrateSilentMode",
                                      QVariant::fromValue(enabled));
    Q_EMIT(incomingCallVibrateSilentModeChanged());
}

bool Sound::getIncomingMessageVibrate()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "IncomingMessageVibrate").toBool();
}

void Sound::setIncomingMessageVibrate(bool enabled)
{
    if (enabled == getIncomingMessageVibrate())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "IncomingMessageVibrate",
                                      QVariant::fromValue(enabled));
    Q_EMIT(incomingMessageVibrateChanged());
}

bool Sound::getIncomingMessageVibrateSilentMode()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "IncomingMessageVibrateSilentMode").toBool();
}

void Sound::setIncomingMessageVibrateSilentMode(bool enabled)
{
    if (enabled == getIncomingMessageVibrateSilentMode())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "IncomingMessageVibrateSilentMode",
                                      QVariant::fromValue(enabled));
    Q_EMIT(incomingMessageVibrateSilentModeChanged());
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
