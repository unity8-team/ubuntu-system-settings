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

Sound::Sound(QObject *parent) :
    QObject(parent)
{

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
