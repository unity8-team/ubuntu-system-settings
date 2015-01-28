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

    if (property == "IncomingCallSound") {
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
    } else if (property == "OtherVibrate") {
        Q_EMIT otherVibrateChanged();
    } else if (property == "DialpadSoundsEnabled") {
        Q_EMIT dialpadSoundsEnabledChanged();
    }
}

void Sound::slotNameOwnerChanged()
{
    // Tell QML so that it refreshes its view of the property
    Q_EMIT incomingCallSoundChanged();
    Q_EMIT incomingMessageSoundChanged();
    Q_EMIT incomingCallVibrateChanged();
    Q_EMIT incomingMessageVibrateChanged();
    Q_EMIT incomingCallVibrateSilentModeChanged();
    Q_EMIT incomingMessageVibrateSilentModeChanged();
    Q_EMIT otherVibrateChanged();
    Q_EMIT dialpadSoundsEnabledChanged();
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

bool Sound::getOtherVibrate()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "OtherVibrate").toBool();
}

void Sound::setOtherVibrate(bool enabled)
{
    if (enabled == getOtherVibrate())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "OtherVibrate",
                                      QVariant::fromValue(enabled));
    Q_EMIT(otherVibrateChanged());
}

bool Sound::getDialpadSoundsEnabled()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "DialpadSoundsEnabled").toBool();
}

void Sound::setDialpadSoundsEnabled(bool enabled)
{
    if (enabled == getDialpadSoundsEnabled())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "DialpadSoundsEnabled",
                                      QVariant::fromValue(enabled));
    Q_EMIT(dialpadSoundsEnabledChanged());
}

QStringList soundsListFromDir(const QString &dirString)
{
    QDir soundsDir(dirString);

    if (soundsDir.exists())
    {
        QStringList soundsList;

        soundsDir.setFilter(QDir::Files | QDir::NoSymLinks);

        for (uint i=0; i < soundsDir.count(); i++)
            soundsList.append(soundsDir.absoluteFilePath(soundsDir[i])) ;
        return soundsList;
    }
    return QStringList();
}

bool sortSoundsList(const QString &s1, const QString &s2)
 {
    return QFileInfo(s1).fileName() < QFileInfo(s2).fileName();
}

/* List soundfiles in a directory and the corresponding /custom one,
 * which is what is used for oem customization, return a list of
 * the fullpaths to those sounds, sorted by name */
QStringList Sound::listSounds(const QStringList &dirs)
{
    QStringList sounds;

    for (int i = 0; i < dirs.size(); ++i)
        sounds.append(soundsListFromDir(dirs[i]));

    qSort(sounds.begin(), sounds.end(), sortSoundsList);

    return sounds;
}
