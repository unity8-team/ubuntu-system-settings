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
#include "keyboard-plugin.h"

#define ORG "maliit.org"
#define APP "server"

#define PLUGIN_DIR "/usr/share/maliit/plugins/com/ubuntu/languages"

#define PLUGIN_PREFIX "libubuntu-keyboard-plugin.so:"
#define MALIIT_PREFIX "maliit/pluginsettings/libmaliit-keyboard-plugin.so"
#define UBUNTU_PREFIX "maliit/pluginsettings/libubuntu-keyboard-plugin.so"

#define KEY_PLUGINS        "maliit/onscreen/enabled"
#define KEY_CAPITALIZATION "auto_caps_enabled"
#define KEY_COMPLETION     "word_engine_enabled"
#define KEY_CORRECTION     "auto_correct_enabled"

static QSettings *maliitSettings;

static QList<QLocale> *languageLocales;
static QHash<QLocale::Language, unsigned int> *languageIndices;
static QStringList *languageNames;

static QList<KeyboardPlugin *> *keyboardPlugins;
static QStringList *pluginNames;
static QList<int> *pluginIndices;

static QSettings *
getMaliitSettings()
{
    if (maliitSettings == NULL)
        maliitSettings = new QSettings("maliit.org", "server");

    return maliitSettings;
}

static bool
compareLocales(const QLocale &locale0,
               const QLocale &locale1)
{
    QString name0(locale0.nativeLanguageName().trimmed().toCaseFolded());
    QString name1(locale1.nativeLanguageName().trimmed().toCaseFolded());

    return name0 < name1;
}

static bool
comparePlugins(KeyboardPlugin *plugin0,
               KeyboardPlugin *plugin1)
{
    const QString &displayName0(plugin0->displayName());
    const QString &displayName1(plugin1->displayName());
    const QString &language0(plugin0->language());
    const QString &language1(plugin1->language());
    const QString &name0(plugin0->name());
    const QString &name1(plugin1->name());

    return displayName0 < displayName1 || (displayName0 == displayName1 && (language0 < language1 || (language0 == language1 && name0 < name1)));
}

static QList<QLocale> *
getLanguageLocales()
{
    if (languageLocales == NULL)
    {
        languageLocales = new QList<QLocale>;

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
                *languageLocales += QLocale(language);
                allLanguages += language;
            }
        }

        qSort(languageLocales->begin(), languageLocales->end(), compareLocales);
    }

    return languageLocales;
}

static QHash<QLocale::Language, unsigned int> *
getLanguageIndices()
{
    if (languageIndices == NULL)
    {
        languageIndices = new QHash<QLocale::Language, unsigned int>;

        for (int i = 0; i < getLanguageLocales()->length(); i++)
            (*languageIndices)[(*getLanguageLocales())[i].language()] = i;
    }

    return languageIndices;
}

static QStringList *
getLanguageNames()
{
    if (languageNames == NULL)
    {
        languageNames = new QStringList;

        for (QList<QLocale>::const_iterator i = getLanguageLocales()->begin(); i != getLanguageLocales()->end(); ++i)
            *languageNames += i->nativeLanguageName().trimmed();
    }

    return languageNames;
}

static QList<KeyboardPlugin *> *
getKeyboardPlugins()
{
    if (keyboardPlugins == NULL)
    {
        keyboardPlugins = new QList<KeyboardPlugin *>;

        QDir pluginDir(PLUGIN_DIR);

        pluginDir.setFilter(QDir::Files);
        pluginDir.setNameFilters(QStringList("*.xml"));
        pluginDir.setSorting(QDir::Name);

        QFileInfoList fileInfoList(pluginDir.entryInfoList());

        for (QFileInfoList::const_iterator i = fileInfoList.begin(); i != fileInfoList.end(); ++i)
        {
            KeyboardPlugin *plugin = new KeyboardPlugin(*i);

            if (!plugin->language().isEmpty())
                *keyboardPlugins += plugin;
        }

        qSort(keyboardPlugins->begin(), keyboardPlugins->end(), comparePlugins);
    }

    return keyboardPlugins;
}

static QStringList *
getPluginNames()
{
    if (pluginNames == NULL)
    {
        pluginNames = new QStringList;

        for (QList<KeyboardPlugin *>::const_iterator i = getKeyboardPlugins()->begin(); i != getKeyboardPlugins()->end(); ++i)
        {
            if (!(*i)->displayName().isEmpty())
                *pluginNames += (*i)->displayName();
            else
                *pluginNames += (*i)->name();
        }
    }

    return pluginNames;
}

static QList<int> *
getPluginIndices()
{
    if (pluginIndices == NULL)
    {
        pluginIndices = new QList<int>;

        QStringList pluginNames = getMaliitSettings()->value(KEY_PLUGINS).toStringList();

        for (QStringList::const_iterator i = pluginNames.begin(); i != pluginNames.end(); ++i)
        {
            if (i->startsWith(PLUGIN_PREFIX))
            {
                QString plugin(i->right(i->length() - strlen(PLUGIN_PREFIX)));

                for (int j = 0; j < getKeyboardPlugins()->length(); j++)
                {
                    if ((*getKeyboardPlugins())[j]->name() == plugin)
                    {
                        *pluginIndices += j;
                        break;
                    }
                }
            }
        }
    }

    return pluginIndices;
}

LanguagePlugin::LanguagePlugin(QObject *parent) :
    QObject(parent)
{
    /* TODO: remove this */
    getKeyboardPlugins();
}

const QStringList &
LanguagePlugin::languages() const
{
    return *getLanguageNames();
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

        act_user_set_language(user, qPrintable((*getLanguageLocales())[index].name()));
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
    if (index >= 0 && index < getLanguageLocales()->length())
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

const QStringList &
LanguagePlugin::plugins() const
{
    return *getPluginNames();
}

const QList<int> &
LanguagePlugin::currentPlugins() const
{
    return *getPluginIndices();
}

void
LanguagePlugin::setCurrentPlugins(const QList<int> &list)
{
    QStringList currentPlugins;

    for (QList<int>::const_iterator i = list.begin(); i != list.end(); ++i)
        if (0 <= *i && *i < getKeyboardPlugins()->length())
            currentPlugins += PLUGIN_PREFIX + (*getKeyboardPlugins())[*i]->name();

    getMaliitSettings()->setValue(KEY_PLUGINS, currentPlugins);

    delete pluginIndices;
    pluginIndices = NULL;
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
