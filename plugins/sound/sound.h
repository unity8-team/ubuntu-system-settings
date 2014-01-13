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
    ~Sound();
    Q_INVOKABLE QStringList listSounds(const QString &dirString);
    Q_PROPERTY (QString incomingCallSound
                READ getIncomingCallSound
                WRITE setIncomingCallSound
                NOTIFY incomingCallSoundChanged)
    Q_PROPERTY (QString incomingMessageSound
                READ getIncomingMessageSound
                WRITE setIncomingMessageSound
                NOTIFY incomingMessageSoundChanged)
    Q_PROPERTY (bool silentMode
                READ getSilentMode
                WRITE setSilentMode
                NOTIFY silentModeChanged)


public Q_SLOTS:
    void slotChanged(QString, QString);
    void slotNameOwnerChanged();

Q_SIGNALS:
    void incomingCallSoundChanged();
    void incomingMessageSoundChanged();
    void silentModeChanged();

private:
    AccountsService m_accountsService;
    QStringList m_soundsList;

    QString getIncomingCallSound();
    void setIncomingCallSound(QString sound);
    QString getIncomingMessageSound();
    void setIncomingMessageSound(QString sound);
    bool getSilentMode();
    void setSilentMode(bool enabled);
};

#endif // SOUND_H
