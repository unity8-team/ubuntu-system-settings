/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sound-plugin.h"

#include <QProcessEnvironment>
#include <QDebug>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

class SoundItem: public ItemBase
{
    Q_OBJECT
public:
    explicit SoundItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);
};

SoundItem::SoundItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    setVisibility(!env.contains(QLatin1String("SNAP")));
}

void SoundItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *SoundPlugin::createItem(const QVariantMap &staticData,
                                  QObject *parent)
{
    return new SoundItem(staticData, parent);
}

#include "sound-plugin.moc"
