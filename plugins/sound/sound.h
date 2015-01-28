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

#ifndef SOUND_H
#define SOUND_H

#include "accountsservice.h"

#include <QObject>
#include <QProcess>

class Sound : public QObject
{
    Q_OBJECT

public:
    explicit Sound(QObject *parent = 0);
    Q_INVOKABLE QStringList listSounds(const QStringList &dirs);
    Q_PROPERTY (QString incomingCallSound
                READ getIncomingCallSound
                WRITE setIncomingCallSound
                NOTIFY incomingCallSoundChanged)
    Q_PROPERTY (QString incomingMessageSound
                READ getIncomingMessageSound
                WRITE setIncomingMessageSound
                NOTIFY incomingMessageSoundChanged)
    Q_PROPERTY (bool incomingCallVibrate
                READ getIncomingCallVibrate
                WRITE setIncomingCallVibrate
                NOTIFY incomingCallVibrateChanged)
    Q_PROPERTY (bool incomingMessageVibrate
                READ getIncomingMessageVibrate
                WRITE setIncomingMessageVibrate
                NOTIFY incomingMessageVibrateChanged)
    Q_PROPERTY (bool incomingCallVibrateSilentMode
                READ getIncomingCallVibrateSilentMode
                WRITE setIncomingCallVibrateSilentMode
                NOTIFY incomingCallVibrateSilentModeChanged)
    Q_PROPERTY (bool incomingMessageVibrateSilentMode
                READ getIncomingMessageVibrateSilentMode
                WRITE setIncomingMessageVibrateSilentMode
                NOTIFY incomingMessageVibrateSilentModeChanged)
    Q_PROPERTY (bool otherVibrate
                READ getOtherVibrate
                WRITE setOtherVibrate
                NOTIFY otherVibrateChanged)

    Q_PROPERTY (bool dialpadSoundsEnabled
                READ getDialpadSoundsEnabled
                WRITE setDialpadSoundsEnabled
                NOTIFY dialpadSoundsEnabledChanged)


public Q_SLOTS:
    void slotChanged(QString, QString);
    void slotNameOwnerChanged();

Q_SIGNALS:
    void incomingCallSoundChanged();
    void incomingMessageSoundChanged();
    void incomingCallVibrateChanged();
    void incomingMessageVibrateChanged();
    void incomingCallVibrateSilentModeChanged();
    void incomingMessageVibrateSilentModeChanged();
    void otherVibrateChanged();
    void dialpadSoundsEnabledChanged();

private:
    AccountsService m_accountsService;

    QString getIncomingCallSound();
    void setIncomingCallSound(QString sound);
    QString getIncomingMessageSound();
    void setIncomingMessageSound(QString sound);
    bool getIncomingCallVibrate();
    void setIncomingCallVibrate(bool enabled);
    bool getIncomingMessageVibrate();
    void setIncomingMessageVibrate(bool enabled);
    bool getIncomingCallVibrateSilentMode();
    void setIncomingCallVibrateSilentMode(bool enabled);
    bool getIncomingMessageVibrateSilentMode();
    void setIncomingMessageVibrateSilentMode(bool enabled);
    bool getOtherVibrate();
    void setOtherVibrate(bool enabled);
    bool getDialpadSoundsEnabled();
    void setDialpadSoundsEnabled(bool enabled);

};

#endif // SOUND_H
