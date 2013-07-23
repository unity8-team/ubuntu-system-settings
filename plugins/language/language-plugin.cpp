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
#include <act/act.h>

static QHash<QString, QLocale::Language> *languageLookup;
static QStringList *languageNames;

static QHash<QString, QLocale::Language> *
getLanguageLookup()
{
    if (languageLookup == NULL)
    {
        languageLookup = new QHash<QString, QLocale::Language>;

        QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage,
                                                          QLocale::AnyScript,
                                                          QLocale::AnyCountry);

        for (QList<QLocale>::const_iterator i = locales.begin(); i != locales.end(); ++i)
        {
            QString name = i->nativeLanguageName().trimmed();

            if (!name.isEmpty())
                (*languageLookup)[name] = i->language();
        }
    }

    return languageLookup;
}

static QStringList *
getLanguageNames()
{
    if (languageNames == NULL)
    {
        languageNames = new QStringList(getLanguageLookup()->keys());
        languageNames->sort(Qt::CaseInsensitive);
    }

    return languageNames;
}

static QLocale::Language
getLanguageForName(const QString &name)
{
    return (*getLanguageLookup())[name];
}

LanguagePlugin::LanguagePlugin(QObject *parent) : QObject(parent)
{
}

const QStringList &
LanguagePlugin::languages() const
{
    return *getLanguageNames();
}

static void
setLanguageWithUser(GObject    *object,
                    GParamSpec *pspec,
                    gpointer    user_data)
{
    Q_UNUSED(pspec);

    ActUser *user = ACT_USER(object);
    QString *name = static_cast<QString *>(user_data);

    if (act_user_is_loaded(user))
    {
        g_signal_handlers_disconnect_by_data(user, name);

        QLocale locale(getLanguageForName(*name));
        act_user_set_language(user, qPrintable(locale.name()));

        delete name;
    }
}

static void
setLanguageWithManager(GObject    *object,
                       GParamSpec *pspec,
                       gpointer    user_data)
{
    Q_UNUSED(pspec);

    ActUserManager *manager = ACT_USER_MANAGER(object);
    QString *language = static_cast<QString *>(user_data);

    gboolean loaded;
    g_object_get(manager, "is-loaded", &loaded, NULL);

    if (loaded)
    {
        g_signal_handlers_disconnect_by_data(manager, language);

        const char *name = qgetenv("USER").constData();

        if (name != NULL && name[0] != '\0')
        {
            ActUser *user = act_user_manager_get_user(manager, name);

            if (user != NULL)
            {
                if (act_user_is_loaded(user))
                    setLanguageWithUser(G_OBJECT(user), NULL, language);
                else
                    g_signal_connect(user, "notify::is-loaded", G_CALLBACK(setLanguageWithUser), language);
            }
            else
                delete language;
        }
        else
            delete language;
    }
}

void
LanguagePlugin::setLanguage(const QString &language)
{
    ActUserManager *manager = act_user_manager_get_default();

    if (manager != NULL)
    {
        gboolean loaded;
        g_object_get(manager, "is-loaded", &loaded, NULL);

        if (loaded)
            setLanguageWithManager(G_OBJECT(manager), NULL, new QString(language));
        else
            g_signal_connect(manager, "notify::is-loaded", G_CALLBACK(setLanguageWithManager), new QString(language));
    }
}
