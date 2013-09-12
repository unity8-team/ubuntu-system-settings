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

#define UBUNTU_KEYBOARD_SCHEMA_ID "com.canonical.keyboard.maliit"

#define KEY_ENABLED_LANGUAGES   "enabled-languages"
#define KEY_AUTO_CAPITALIZATION "auto-capitalization"
#define KEY_AUTO_COMPLETION     "auto-completion"
#define KEY_PREDICTIVE_TEXT     "predictive-text"
#define KEY_KEY_PRESS_FEEDBACK  "key-press-feedback"

#define LANGUAGE_DIR "/usr/share/maliit/plugins/com/ubuntu/languages"

static QList<QLocale> *languageLocales;
static QHash<QLocale::Language, unsigned int> *languageIndices;
static QStringList *languageNames;

static GSettings *maliitSettings;
static QList<KeyboardPlugin *> *keyboardPlugins;
static SubsetModel *pluginsModel;

static bool
compareLocales(const QLocale &locale0,
               const QLocale &locale1)
{
    QString name0(locale0.nativeLanguageName().trimmed().toCaseFolded());
    QString name1(locale1.nativeLanguageName().trimmed().toCaseFolded());

    return name0 < name1;
}

static bool
comparePlugins(const KeyboardPlugin *plugin0,
               const KeyboardPlugin *plugin1)
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
    if (languageLocales == NULL) {
        languageLocales = new QList<QLocale>;

        QSet<QLocale::Language> allLanguages;
        QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage,
                                                             QLocale::AnyScript,
                                                             QLocale::AnyCountry);

        for (QList<QLocale>::const_iterator i = allLocales.begin(); i != allLocales.end(); ++i) {
            QLocale::Language language = i->language();

            bool uniqueLanguage = language != QLocale::AnyLanguage &&
                !i->nativeLanguageName().trimmed().toCaseFolded().isEmpty() &&
                !allLanguages.contains(language);

            if (uniqueLanguage) {
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
    if (languageIndices == NULL) {
        languageIndices = new QHash<QLocale::Language, unsigned int>;

        for (int i = 0; i < getLanguageLocales()->length(); i++)
            (*languageIndices)[(*getLanguageLocales())[i].language()] = i;
    }

    return languageIndices;
}

static QStringList *
getLanguageNames()
{
    if (languageNames == NULL) {
        languageNames = new QStringList;

        for (QList<QLocale>::const_iterator i = getLanguageLocales()->begin(); i != getLanguageLocales()->end(); ++i)
            *languageNames += i->nativeLanguageName().trimmed();
    }

    return languageNames;
}

static GSettings *
getMaliitSettings()
{
    if (maliitSettings == NULL)
        maliitSettings = g_settings_new(UBUNTU_KEYBOARD_SCHEMA_ID);

    return maliitSettings;
}

static QList<KeyboardPlugin *> *
getKeyboardPlugins()
{
    if (keyboardPlugins == NULL) {
        keyboardPlugins = new QList<KeyboardPlugin *>;

        QDir pluginDir(LANGUAGE_DIR);

        pluginDir.setFilter(QDir::Files);
        pluginDir.setNameFilters(QStringList("*.xml"));
        pluginDir.setSorting(QDir::Name);

        QFileInfoList fileInfoList(pluginDir.entryInfoList());

        for (QFileInfoList::const_iterator i = fileInfoList.begin(); i != fileInfoList.end(); ++i) {
            KeyboardPlugin *plugin = new KeyboardPlugin(*i);

            if (!plugin->language().isEmpty())
                *keyboardPlugins += plugin;
        }

        qSort(keyboardPlugins->begin(), keyboardPlugins->end(), comparePlugins);
    }

    return keyboardPlugins;
}

static SubsetModel *
getPluginsModel()
{
    if (pluginsModel == NULL) {
        QStringList pluginNames;

        for (QList<KeyboardPlugin *>::const_iterator i = getKeyboardPlugins()->begin(); i != getKeyboardPlugins()->end(); ++i) {
            if (!(*i)->displayName().isEmpty())
                pluginNames += (*i)->displayName();
            else
                pluginNames += (*i)->name();
        }

        GVariantIter *iter;
        const gchar *language;
        QList<int> pluginIndices;

        g_settings_get(getMaliitSettings(), KEY_ENABLED_LANGUAGES, "as", &iter);

        while (g_variant_iter_next(iter, "&s", &language)) {
            for (int i = 0; i < getKeyboardPlugins()->length(); i++) {
                if ((*getKeyboardPlugins())[i]->name() == language) {
                    pluginIndices += i;
                    break;
                }
            }
        }

        g_variant_iter_free(iter);

        pluginsModel = new SubsetModel();
        pluginsModel->setSuperset(pluginNames);
        pluginsModel->setSubset(pluginIndices);
    }

    return pluginsModel;
}

LanguagePlugin::LanguagePlugin(QObject *parent) :
    QObject(parent),
    _updatePluginsConnected(false)
{
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

    if (act_user_is_loaded(user)) {
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

    if (loaded) {
        g_signal_handlers_disconnect_by_data(manager, user_data);

        const char *name = qPrintable(qgetenv("USER"));

        if (name != NULL && name[0] != '\0') {
            ActUser *user = act_user_manager_get_user(manager, name);

            if (user != NULL) {
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
    if (index >= 0 && index < getLanguageLocales()->length()) {
        ActUserManager *manager = act_user_manager_get_default();

        if (manager != NULL) {
            gboolean loaded;
            g_object_get(manager, "is-loaded", &loaded, NULL);

            if (loaded)
                setLanguageWithManager(G_OBJECT(manager), NULL, GINT_TO_POINTER(index));
            else
                g_signal_connect(manager, "notify::is-loaded", G_CALLBACK(setLanguageWithManager), GINT_TO_POINTER(index));

            Q_EMIT currentLanguageChanged();
        }
    }
}

SubsetModel *
LanguagePlugin::pluginsModel()
{
    if (!_updatePluginsConnected) {
        connect(getPluginsModel(), SIGNAL(subsetChanged()), SLOT(updatePlugins()));

        _updatePluginsConnected = true;
    }

    return getPluginsModel();
}

void
LanguagePlugin::updatePlugins()
{
    GVariantBuilder builder;
    GVariant *currentPlugins;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));

    for (QList<int>::const_iterator i = pluginsModel()->subset().begin(); i != pluginsModel()->subset().end(); ++i)
        g_variant_builder_add(&builder, "s", qPrintable((*getKeyboardPlugins())[*i]->name()));

    currentPlugins = g_variant_ref_sink(g_variant_builder_end(&builder));
    g_settings_set_value(getMaliitSettings(), KEY_ENABLED_LANGUAGES, currentPlugins);
    g_variant_unref(currentPlugins);
}

bool
LanguagePlugin::autoCapitalization() const
{
    return g_settings_get_boolean(getMaliitSettings(), KEY_AUTO_CAPITALIZATION);
}

void
LanguagePlugin::setAutoCapitalization(bool value)
{
    if (value != autoCapitalization()) {
        g_settings_set_boolean(getMaliitSettings(), KEY_AUTO_CAPITALIZATION, value);
        Q_EMIT autoCapitalizationChanged();
    }
}

bool
LanguagePlugin::autoCompletion() const
{
    return g_settings_get_boolean(getMaliitSettings(), KEY_AUTO_COMPLETION);
}

void
LanguagePlugin::setAutoCompletion(bool value)
{
    if (value != autoCompletion()) {
        g_settings_set_boolean(getMaliitSettings(), KEY_AUTO_COMPLETION, value);
        Q_EMIT autoCompletionChanged();
    }
}

bool
LanguagePlugin::predictiveText() const
{
    return g_settings_get_boolean(getMaliitSettings(), KEY_PREDICTIVE_TEXT);
}

void
LanguagePlugin::setPredictiveText(bool value)
{
    if (value != predictiveText()) {
        g_settings_set_boolean(getMaliitSettings(), KEY_PREDICTIVE_TEXT, value);
        Q_EMIT predictiveTextChanged();
    }
}

bool
LanguagePlugin::keyPressFeedback() const
{
    return g_settings_get_boolean(getMaliitSettings(), KEY_KEY_PRESS_FEEDBACK);
}

void
LanguagePlugin::setKeyPressFeedback(bool value)
{
    if (value != keyPressFeedback()) {
        g_settings_set_boolean(getMaliitSettings(), KEY_KEY_PRESS_FEEDBACK, value);
        Q_EMIT keyPressFeedbackChanged();
    }
}
