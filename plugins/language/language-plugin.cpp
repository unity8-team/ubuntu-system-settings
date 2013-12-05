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
#include "keyboard-layout.h"

#define UBUNTU_KEYBOARD_SCHEMA_ID "com.canonical.keyboard.maliit"

#define KEY_ENABLED_LAYOUTS     "enabled-languages"
#define KEY_AUTO_CAPITALIZATION "auto-capitalization"
#define KEY_AUTO_COMPLETION     "auto-completion"
#define KEY_PREDICTIVE_TEXT     "predictive-text"
#define KEY_KEY_PRESS_FEEDBACK  "key-press-feedback"

#define LAYOUTS_DIR "/usr/share/maliit/plugins/com/ubuntu/languages"

void managerLoaded(GObject    *object,
                   GParamSpec *pspec,
                   gpointer    user_data);

LanguagePlugin::LanguagePlugin(QObject *parent) :
    QObject(parent),
    m_currentLanguage(-1),
    m_nextCurrentLanguage(-1),
    m_manager(act_user_manager_get_default()),
    m_user(NULL),
    m_maliitSettings(g_settings_new(UBUNTU_KEYBOARD_SCHEMA_ID))
{
    if (m_manager != NULL) {
        g_object_ref(m_manager);

        gboolean loaded;
        g_object_get(m_manager, "is-loaded", &loaded, NULL);

        if (loaded)
            managerLoaded();
        else
            g_signal_connect(m_manager, "notify::is-loaded", G_CALLBACK(::managerLoaded), this);
    }

    updateLanguageLocales();
    updateCurrentLanguage();
    updateKeyboardLayouts();
    updateKeyboardLayoutsModel();
    updateSpellCheckingModel();
}

LanguagePlugin::~LanguagePlugin()
{
    if (m_user != NULL) {
        g_signal_handlers_disconnect_by_data(m_user, this);
        g_object_unref(m_user);
    }

    if (m_manager != NULL) {
        g_signal_handlers_disconnect_by_data(m_manager, this);
        g_object_unref(m_manager);
    }

    if (m_maliitSettings != NULL) {
        g_signal_handlers_disconnect_by_data(m_maliitSettings, this);
        g_object_unref(m_maliitSettings);
    }

    for (QList<KeyboardLayout *>::const_iterator i(m_keyboardLayouts.begin()); i != m_keyboardLayouts.end(); ++i)
        delete *i;
}

const QStringList &
LanguagePlugin::languageNames() const
{
    return m_languageNames;
}

const QStringList &
LanguagePlugin::languageCodes() const
{
    return m_languageCodes;
}

int
LanguagePlugin::currentLanguage() const
{
    return m_currentLanguage;
}

void
LanguagePlugin::setCurrentLanguage(int index)
{
    if (index >= 0 && index < m_languageLocales.length()) {
        m_nextCurrentLanguage = index;

        updateCurrentLanguage();
    }
}

SubsetModel *
LanguagePlugin::keyboardLayoutsModel()
{
    return &m_keyboardLayoutsModel;
}

void
LanguagePlugin::keyboardLayoutsModelChanged()
{
    GVariantBuilder builder;
    GVariant *currentLayouts;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));

    for (QList<int>::const_iterator i(m_keyboardLayoutsModel.subset().begin()); i != m_keyboardLayoutsModel.subset().end(); ++i)
        g_variant_builder_add(&builder, "s", qPrintable(m_keyboardLayouts[*i]->name()));

    currentLayouts = g_variant_ref_sink(g_variant_builder_end(&builder));
    g_settings_set_value(m_maliitSettings, KEY_ENABLED_LAYOUTS, currentLayouts);
    g_variant_unref(currentLayouts);
}

bool
LanguagePlugin::spellChecking() const
{
    // TODO: get spell checking setting
    return true;
}

void
LanguagePlugin::setSpellChecking(bool value)
{
    // TODO: set spell checking setting
    Q_UNUSED(value);
}

SubsetModel *
LanguagePlugin::spellCheckingModel()
{
    return &m_spellCheckingModel;
}

void
LanguagePlugin::spellCheckingModelChanged()
{
    // TODO: update spell checking model
}

bool
LanguagePlugin::autoCapitalization() const
{
    return g_settings_get_boolean(m_maliitSettings, KEY_AUTO_CAPITALIZATION);
}

void
LanguagePlugin::setAutoCapitalization(bool value)
{
    if (value != autoCapitalization()) {
        g_settings_set_boolean(m_maliitSettings, KEY_AUTO_CAPITALIZATION, value);
        Q_EMIT autoCapitalizationChanged();
    }
}

bool
LanguagePlugin::autoCompletion() const
{
    return g_settings_get_boolean(m_maliitSettings, KEY_AUTO_COMPLETION);
}

void
LanguagePlugin::setAutoCompletion(bool value)
{
    if (value != autoCompletion()) {
        g_settings_set_boolean(m_maliitSettings, KEY_AUTO_COMPLETION, value);
        Q_EMIT autoCompletionChanged();
    }
}

bool
LanguagePlugin::predictiveText() const
{
    return g_settings_get_boolean(m_maliitSettings, KEY_PREDICTIVE_TEXT);
}

void
LanguagePlugin::setPredictiveText(bool value)
{
    if (value != predictiveText()) {
        g_settings_set_boolean(m_maliitSettings, KEY_PREDICTIVE_TEXT, value);
        Q_EMIT predictiveTextChanged();
    }
}

bool
LanguagePlugin::keyPressFeedback() const
{
    return g_settings_get_boolean(m_maliitSettings, KEY_KEY_PRESS_FEEDBACK);
}

void
LanguagePlugin::setKeyPressFeedback(bool value)
{
    if (value != keyPressFeedback()) {
        g_settings_set_boolean(m_maliitSettings, KEY_KEY_PRESS_FEEDBACK, value);
        Q_EMIT keyPressFeedbackChanged();
    }
}

static bool
compareLocales(const QLocale &locale0,
               const QLocale &locale1)
{
    QString name0(locale0.bcp47Name());
    QString name1(locale1.bcp47Name());

    return name0 < name1;
}

static bool
compareLayouts(const KeyboardLayout *layout0,
               const KeyboardLayout *layout1)
{
    QString name0(layout0->displayName());
    QString name1(layout1->displayName());

    if (name0 == name1) {
        name0 = layout0->language();
        name1 = layout1->language();

        if (name0 == name1) {
            name0 = layout0->name();
            name1 = layout1->name();
        }
    }

    return name0 < name1;
}

void
LanguagePlugin::updateLanguageLocales()
{
    m_languageLocales.clear();
    m_languageNames.clear();
    m_languageCodes.clear();
    m_indicesByBcp47Name.clear();
    m_indicesByLocaleName.clear();

    QProcess localeProcess;
    localeProcess.start("locale", QStringList("-a"), QIODevice::ReadOnly);
    localeProcess.waitForFinished();

    QString localeOutput(localeProcess.readAllStandardOutput());
    QStringList localeNames(localeOutput.split(QRegExp("\\s+")));
    QHash<QString, QString> localeNamesByBcp47Name;

    for (QStringList::const_iterator i(localeNames.begin()); i != localeNames.end(); ++i) {
        QLocale locale(i->left(i->indexOf('.')).left(i->indexOf('_')));
        QString language(locale.nativeLanguageName());
        QString bcp47Name(locale.bcp47Name());

        if (!language.isEmpty() && !localeNamesByBcp47Name.contains(bcp47Name)) {
            m_languageLocales += locale;

            for (QStringList::const_iterator j(i); j != localeNames.end(); ++j) {
                QLocale likelyLocale(j->left(j->indexOf('.')));
                QString likelyBcp47Name(likelyLocale.bcp47Name());

                if (likelyBcp47Name == bcp47Name) {
                    localeNamesByBcp47Name[bcp47Name] = *j;
                    break;
                }
            }

            if (!localeNamesByBcp47Name.contains(bcp47Name)) {
                int index(i->indexOf('.'));

                if (index < 0) {
                    localeNamesByBcp47Name[bcp47Name] = i->left(index).left(i->indexOf('_'));
                } else {
                    localeNamesByBcp47Name[bcp47Name] = i->left(index).left(i->indexOf('_')) + i->right(i->length() - index);
                }
            }
        }

        locale = QLocale(i->left(i->indexOf('.')));
        language = locale.nativeLanguageName();
        bcp47Name = locale.bcp47Name();

        if (!language.isEmpty() && !localeNamesByBcp47Name.contains(bcp47Name)) {
            m_languageLocales += locale;
            localeNamesByBcp47Name[bcp47Name] = *i;
        }
    }

    qSort(m_languageLocales.begin(), m_languageLocales.end(), compareLocales);

    for (int i(0); i < m_languageLocales.length(); i++) {
        const QLocale &locale(m_languageLocales[i]);
        QString language(locale.nativeLanguageName());
        QString country(locale.nativeCountryName());
        QString bcp47Name(locale.bcp47Name());

        m_languageNames += QString("%1 (%2)").arg(language).arg(country);
        m_languageCodes += localeNamesByBcp47Name[bcp47Name];
        m_indicesByBcp47Name[bcp47Name] = i;
        m_indicesByLocaleName[localeNamesByBcp47Name[bcp47Name]] = i;
    }
}

void
LanguagePlugin::updateCurrentLanguage()
{
    int previousLanguage(m_currentLanguage);

    if (m_user != NULL && act_user_is_loaded(m_user)) {
        if (m_nextCurrentLanguage >= 0) {
            m_currentLanguage = m_nextCurrentLanguage;
            m_nextCurrentLanguage = -1;

            QString languageCode(m_languageCodes[m_currentLanguage]);
            act_user_set_language(m_user, qPrintable(languageCode.left(languageCode.indexOf('.'))));
            act_user_set_formats_locale(m_user, qPrintable(languageCode));
        } else {
            const char *language(act_user_get_language(m_user));
            m_currentLanguage = indexForLanguage(language);

            if (m_currentLanguage < 0) {
                QLocale locale(language);
                m_currentLanguage = indexForLocale(locale);

                if (m_currentLanguage < 0) {
                    locale = QLocale(locale.language());
                    m_currentLanguage = indexForLocale(locale);
                }
            }
        }
    }

    if (m_currentLanguage < 0) {
        QLocale locale(QLocale::system());
        m_currentLanguage = indexForLocale(locale);

        if (m_currentLanguage < 0) {
            locale = QLocale(locale.language());
            m_currentLanguage = indexForLocale(locale);
        }
    }

    if (m_currentLanguage != previousLanguage)
        Q_EMIT currentLanguageChanged();
}

void
LanguagePlugin::updateKeyboardLayouts()
{
    m_keyboardLayouts.clear();

    QDir layoutsDir(LAYOUTS_DIR);
    layoutsDir.setFilter(QDir::Files);
    layoutsDir.setNameFilters(QStringList("*.xml"));
    layoutsDir.setSorting(QDir::Name);

    QFileInfoList fileInfoList(layoutsDir.entryInfoList());

    for (QFileInfoList::const_iterator i(fileInfoList.begin()); i != fileInfoList.end(); ++i) {
        KeyboardLayout *layout(new KeyboardLayout(*i));

        if (!layout->language().isEmpty())
            m_keyboardLayouts += layout;
        else
            delete layout;
    }

    qSort(m_keyboardLayouts.begin(), m_keyboardLayouts.end(), compareLayouts);
}

void enabledLayoutsChanged(GSettings *settings,
                           gchar     *key,
                           gpointer   user_data);

void
LanguagePlugin::updateKeyboardLayoutsModel()
{
    QStringList customRoles;
    customRoles += "language";
    customRoles += "icon";

    m_keyboardLayoutsModel.setCustomRoles(customRoles);

    QVariantList superset;

    for (QList<KeyboardLayout *>::const_iterator i(m_keyboardLayouts.begin()); i != m_keyboardLayouts.end(); ++i) {
        QVariantList element;

        if (!(*i)->displayName().isEmpty())
            element += (*i)->displayName();
        else
            element += (*i)->name();

        element += (*i)->shortName();
        superset += QVariant(element);
    }

    m_keyboardLayoutsModel.setSuperset(superset);

    enabledLayoutsChanged();

    m_keyboardLayoutsModel.setAllowEmpty(false);

    connect(&m_keyboardLayoutsModel, SIGNAL(subsetChanged()), SLOT(keyboardLayoutsModelChanged()));

    g_signal_connect(m_maliitSettings, "changed::" KEY_ENABLED_LAYOUTS, G_CALLBACK(::enabledLayoutsChanged), this);
}

void
LanguagePlugin::updateSpellCheckingModel()
{
    // TODO: populate spell checking model
    QVariantList superset;

    for (QStringList::const_iterator i(m_languageNames.begin()); i != m_languageNames.end(); ++i) {
        QVariantList element;
        element += *i;
        superset += QVariant(element);
    }

    m_spellCheckingModel.setCustomRoles(QStringList("language"));
    m_spellCheckingModel.setSuperset(superset);
    m_spellCheckingModel.setSubset(QList<int>());
    m_spellCheckingModel.setAllowEmpty(false);

    connect(&m_spellCheckingModel, SIGNAL(subsetChanged()), SLOT(spellCheckingModelChanged()));
}

int
LanguagePlugin::indexForLocale(const QLocale &locale) const
{
    return m_indicesByBcp47Name.value(locale.bcp47Name(), -1);
}

int
LanguagePlugin::indexForLanguage(const QString &language) const
{
    return m_indicesByLocaleName.value(language, -1);
}

void
LanguagePlugin::userLoaded()
{
    if (act_user_is_loaded(m_user)) {
        g_signal_handlers_disconnect_by_data(m_user, this);

        updateCurrentLanguage();
    }
}

void
userLoaded(GObject    *object,
           GParamSpec *pspec,
           gpointer    user_data)
{
    Q_UNUSED(object);
    Q_UNUSED(pspec);

    LanguagePlugin *plugin(static_cast<LanguagePlugin *>(user_data));
    plugin->userLoaded();
}

void
LanguagePlugin::managerLoaded()
{
    gboolean loaded;
    g_object_get(m_manager, "is-loaded", &loaded, NULL);

    if (loaded) {
        g_signal_handlers_disconnect_by_data(m_manager, this);

        const char *name(qPrintable(qgetenv("USER")));

        if (name != NULL) {
            m_user = act_user_manager_get_user(m_manager, name);

            if (m_user != NULL) {
                g_object_ref(m_user);

                if (act_user_is_loaded(m_user))
                    userLoaded();
                else
                    g_signal_connect(m_user, "notify::is-loaded", G_CALLBACK(::userLoaded), this);
            }
        }
    }
}

void
managerLoaded(GObject    *object,
              GParamSpec *pspec,
              gpointer    user_data)
{
    Q_UNUSED(object);
    Q_UNUSED(pspec);

    LanguagePlugin *plugin(static_cast<LanguagePlugin *>(user_data));
    plugin->managerLoaded();
}

void
LanguagePlugin::enabledLayoutsChanged()
{
    GVariantIter *iter;
    const gchar *language;
    QList<int> subset;

    g_settings_get(m_maliitSettings, KEY_ENABLED_LAYOUTS, "as", &iter);

    while (g_variant_iter_next(iter, "&s", &language)) {
        for (int i(0); i < m_keyboardLayouts.length(); i++) {
            if (m_keyboardLayouts[i]->name() == language) {
                subset += i;
                break;
            }
        }
    }

    g_variant_iter_free(iter);

    m_keyboardLayoutsModel.setSubset(subset);
}

void
enabledLayoutsChanged(GSettings *settings,
                      gchar     *key,
                      gpointer   user_data)
{
    Q_UNUSED(settings);
    Q_UNUSED(key);

    LanguagePlugin *plugin(static_cast<LanguagePlugin *>(user_data));
    plugin->enabledLayoutsChanged();
}
