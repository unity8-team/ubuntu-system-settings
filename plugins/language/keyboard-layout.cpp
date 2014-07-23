/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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

#include "keyboard-layout.h"
#include <unicode/locid.h>

KeyboardLayout::KeyboardLayout(const QString &name,
                               const QString &language,
                               const QString &displayName,
                               const QString &shortName,
                               QObject       *parent) :
    QObject(parent),
    m_name(name),
    m_language(language),
    m_displayName(displayName),
    m_shortName(shortName)
{
}

KeyboardLayout::KeyboardLayout(const QFileInfo &fileInfo,
                               QObject         *parent) :
    QObject(parent),
    m_name(fileInfo.fileName())
{
    icu::Locale locale(qPrintable(m_name));
    icu::UnicodeString unicodeString;
    std::string string;

    locale.getDisplayName(locale, unicodeString);
    unicodeString.toTitle(nullptr, locale).toUTF8String(string);

    m_language = locale.getLanguage();
    m_displayName = string.c_str();
    m_shortName = m_language.left(2);
    m_shortName[0] = m_shortName[0].toUpper();
}

const QString &
KeyboardLayout::name() const
{
    return m_name;
}

const QString &
KeyboardLayout::language() const
{
    return m_language;
}

const QString &
KeyboardLayout::displayName() const
{
    return m_displayName;
}

const QString &
KeyboardLayout::shortName() const
{
    return m_shortName;
}
