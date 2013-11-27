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
#include <QtXmlPatterns>

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
    m_name(fileInfo.completeBaseName())
{
    QVariant path(fileInfo.canonicalFilePath());

    QXmlQuery languageQuery;
    languageQuery.bindVariable("path", path);
    languageQuery.setQuery("xs:string(doc($path)/keyboard/@language)");

    QStringList languageResults;

    if (languageQuery.evaluateTo(&languageResults) && !languageResults.isEmpty())
        m_language = languageResults.first();

    QXmlQuery titleQuery;
    titleQuery.bindVariable("path", path);
    titleQuery.setQuery("xs:string(doc($path)/keyboard/@title)");

    QStringList titleResults;

    if (titleQuery.evaluateTo(&titleResults) && !titleResults.isEmpty())
        m_displayName = titleResults.first();

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
