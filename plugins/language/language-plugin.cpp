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

#include "language-plugin.h"

LanguagePlugin::LanguagePlugin(QObject *parent) : QObject(parent), _languages(NULL)
{
}

LanguagePlugin::~LanguagePlugin()
{
    delete _languages;
}

const QStringList &
LanguagePlugin::languages() const
{
    if (_languages == NULL)
    {
        QSet<QString> set = QSet<QString>();
        QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage,
                                                          QLocale::AnyScript,
                                                          QLocale::AnyCountry);

        for (QList<QLocale>::const_iterator i = locales.begin(); i != locales.end(); ++i)
        {
            QString name = i->nativeLanguageName().trimmed();

            if (!name.isEmpty())
                set += name;
        }

        _languages = new QStringList(set.toList());
        _languages->sort(Qt::CaseInsensitive);
    }

    return *_languages;
}
