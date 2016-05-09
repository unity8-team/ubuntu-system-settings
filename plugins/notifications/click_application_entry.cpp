/*
 * Copyright (C) 2016 Canonical Ltd
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
*/

#include "click_application_entry.h"

ClickApplicationEntry::ClickApplicationEntry(QObject* parent)
    : QObject(parent),
    m_pkgName(),
    m_version(),
    m_appName(),   
    m_displayName(),
    m_icon(),
    m_soundsNotify(true),
    m_vibrationsNotify(true),
    m_bubblesNotify(true),
    m_listNotify(true)
{
}

ClickApplicationEntry::~ClickApplicationEntry()
{
}

void ClickApplicationEntry::setSoundsNotify(bool notify)
{
    if (m_soundsNotify == notify) {
        return;
    }

    m_soundsNotify = notify;
    Q_EMIT soundsNotifyChanged();
}

void ClickApplicationEntry::setVibrationsNotify(bool notify)
{
    if (m_vibrationsNotify == notify) {
        return;
    }

    m_vibrationsNotify = notify;
    Q_EMIT vibrationsNotifyChanged();
}

void ClickApplicationEntry::setBubblesNotify(bool notify)
{
    if (m_bubblesNotify == notify) {
        return;
    }

    m_bubblesNotify = notify;
    Q_EMIT bubblesNotifyChanged();
}

void ClickApplicationEntry::setListNotify(bool notify)
{
    if (m_listNotify == notify) {
        return;
    }

    m_listNotify = notify;
    Q_EMIT listNotifyChanged();
}
