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

#define ORG "maliit.org"
#define APP "server"

#define MALIIT_PREFIX "pluginsettings/libmaliit-keyboard-plugin.so"
#define UBUNTU_PREFIX "pluginsettings/libubuntu-keyboard-plugin.so"

#define KEY_CAPITALIZATION "auto_caps_enabled"
#define KEY_COMPLETION     "word_engine_enabled"
#define KEY_CORRECTION     "auto_correct_enabled"

static QList<QLocale> *locales;
static QHash<QLocale::Language, unsigned int> *languageIndices;
static QStringList *localeNames;

static QSettings *maliitSettings;

static bool
compareLocales(const QLocale &locale0,
               const QLocale &locale1)
{
    QString name0(locale0.nativeLanguageName().trimmed().toCaseFolded());
    QString name1(locale1.nativeLanguageName().trimmed().toCaseFolded());

    return name0 < name1;
}

static QList<QLocale> *
getLocales()
{
    if (locales == NULL)
    {
        locales = new QList<QLocale>;

        QSet<QLocale::Language> allLanguages;
        QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage,
                                                             QLocale::AnyScript,
                                                             QLocale::AnyCountry);

        for (QList<QLocale>::const_iterator i = allLocales.begin(); i != allLocales.end(); ++i)
        {
            QLocale::Language language = i->language();

            if (language != QLocale::AnyLanguage &&
                !i->nativeLanguageName().trimmed().toCaseFolded().isEmpty() &&
                !allLanguages.contains(language))
            {
                *locales += QLocale(language);
                allLanguages += language;
            }
        }

        qSort(locales->begin(), locales->end(), compareLocales);
    }

    return locales;
}

static QHash<QLocale::Language, unsigned int> *
getLanguageIndices()
{
    if (languageIndices == NULL)
    {
        languageIndices = new QHash<QLocale::Language, unsigned int>;

        for (int i = 0; i < getLocales()->length(); i++)
            (*languageIndices)[(*getLocales())[i].language()] = i;
    }

    return languageIndices;
}

static QStringList *
getLocaleNames()
{
    if (localeNames == NULL)
    {
        localeNames = new QStringList;

        for (QList<QLocale>::const_iterator i = getLocales()->begin(); i != getLocales()->end(); ++i)
            *localeNames += i->nativeLanguageName().trimmed();
    }

    return localeNames;
}

static QSettings *
getMaliitSettings()
{
    if (maliitSettings == NULL)
        maliitSettings = new QSettings("maliit.org", "server");

    qDebug("getMaliitSettings ()");

    return maliitSettings;
}

LanguagePlugin::LanguagePlugin(QObject *parent) : QObject(parent)
{
}

const QStringList &
LanguagePlugin::languages() const
{
    return *getLocaleNames();
}

int
LanguagePlugin::currentLanguage() const
{
    QLocale::Language language = QLocale::system().language();
    QHash<QLocale::Language, unsigned int> *indices = getLanguageIndices();
    QHash<QLocale::Language, unsigned int>::const_iterator i = indices->find(language);

    return i != indices->end() ? *i : -1;
}

static void
setLanguageWithUser(GObject    *object,
                    GParamSpec *pspec,
                    gpointer    user_data)
{
    Q_UNUSED(pspec);

    ActUser *user = ACT_USER(object);
    gint index = GPOINTER_TO_INT(user_data);

    if (act_user_is_loaded(user))
    {
        g_signal_handlers_disconnect_by_data(user, user_data);

        act_user_set_language(user, qPrintable((*getLocales())[index].name()));
    }
}

static void
setLanguageWithManager(GObject    *object,
                       GParamSpec *pspec,
                       gpointer    user_data)
{
    Q_UNUSED(pspec);

    ActUserManager *manager = ACT_USER_MANAGER(object);

    gboolean loaded;
    g_object_get(manager, "is-loaded", &loaded, NULL);

    if (loaded)
    {
        g_signal_handlers_disconnect_by_data(manager, user_data);

        const char *name = qgetenv("USER").constData();

        if (name != NULL && name[0] != '\0')
        {
            ActUser *user = act_user_manager_get_user(manager, name);

            if (user != NULL)
            {
                if (act_user_is_loaded(user))
                    setLanguageWithUser(G_OBJECT(user), NULL, user_data);
                else
                    g_signal_connect(user, "notify::is-loaded", G_CALLBACK(setLanguageWithUser), user_data);
            }
        }
    }
}

void
LanguagePlugin::setCurrentLanguage(int index)
{
    if (index >= 0 && index < getLocales()->length())
    {
        ActUserManager *manager = act_user_manager_get_default();

        if (manager != NULL)
        {
            gboolean loaded;
            g_object_get(manager, "is-loaded", &loaded, NULL);

            if (loaded)
                setLanguageWithManager(G_OBJECT(manager), NULL, GINT_TO_POINTER(index));
            else
                g_signal_connect(manager, "notify::is-loaded", G_CALLBACK(setLanguageWithManager), GINT_TO_POINTER(index));
        }
    }
}

bool
LanguagePlugin::autoCapitalization() const
{
    return getMaliitSettings()->value(UBUNTU_PREFIX "/" KEY_CAPITALIZATION, false).toBool();
}

void
LanguagePlugin::setAutoCapitalization(bool value)
{
    getMaliitSettings()->setValue(MALIIT_PREFIX "/" KEY_CAPITALIZATION, value);
    getMaliitSettings()->setValue(UBUNTU_PREFIX "/" KEY_CAPITALIZATION, value);
    getMaliitSettings()->sync();
}

bool
LanguagePlugin::autoCompletion() const
{
    return getMaliitSettings()->value(UBUNTU_PREFIX "/" KEY_COMPLETION, false).toBool();
}

void
LanguagePlugin::setAutoCompletion(bool value)
{
    getMaliitSettings()->setValue(MALIIT_PREFIX "/" KEY_COMPLETION, value);
    getMaliitSettings()->setValue(UBUNTU_PREFIX "/" KEY_COMPLETION, value);
    getMaliitSettings()->sync();
}

bool
LanguagePlugin::autoCorrection() const
{
    return getMaliitSettings()->value(UBUNTU_PREFIX "/" KEY_CORRECTION, false).toBool();
}

void
LanguagePlugin::setAutoCorrection(bool value)
{
    getMaliitSettings()->setValue(MALIIT_PREFIX "/" KEY_CORRECTION, value);
    getMaliitSettings()->setValue(UBUNTU_PREFIX "/" KEY_CORRECTION, value);
    getMaliitSettings()->sync();
}

bool
LanguagePlugin::autoPunctuation() const
{
    /* TODO: Get auto punctuation setting. */
    return false;
}

void
LanguagePlugin::setAutoPunctuation(bool value)
{
    /* TODO: Set auto punctuation setting. */
    Q_UNUSED(value);
}
