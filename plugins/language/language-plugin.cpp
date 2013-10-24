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

LanguagePlugin::LanguagePlugin(QObject *parent) :
    QObject(parent),
    m_languageLocales(NULL),
    m_languageNames(NULL),
    m_languageCodes(NULL),
    m_indicesByBcp47Name(NULL),
    m_indicesByLocaleName(NULL),
    m_currentLanguage(-1),
    m_nextCurrentLanguage(-1),
    m_manager(NULL),
    m_user(NULL),
    m_maliitSettings(NULL),
    m_keyboardLayouts(NULL),
    m_keyboardLayoutsModel(NULL),
    m_spellCheckingModel(NULL)
{
}

LanguagePlugin::~LanguagePlugin()
{
    delete m_languageLocales;
    delete m_languageNames;
    delete m_languageCodes;
    delete m_indicesByBcp47Name;
    delete m_indicesByLocaleName;

    if (m_manager != NULL) {
        g_signal_handlers_disconnect_by_data(m_manager, this);
        g_object_unref(m_manager);
    }

    if (m_user != NULL) {
        g_signal_handlers_disconnect_by_data(m_user, this);
        g_object_unref(m_user);
    }

    if (m_maliitSettings != NULL)
        g_object_unref(m_maliitSettings);

    for (QList<KeyboardLayout *>::const_iterator i(m_keyboardLayouts->begin()); i != m_keyboardLayouts->end(); ++i)
        delete *i;

    delete m_keyboardLayouts;
    delete m_keyboardLayoutsModel;
    delete m_spellCheckingModel;
}

const QStringList &
LanguagePlugin::languageNames() const
{
    languageLocales();
    return *m_languageNames;
}

const QStringList &
LanguagePlugin::languageCodes() const
{
    languageLocales();
    return *m_languageCodes;
}

int
LanguagePlugin::currentLanguage() const
{
    if (m_currentLanguage < 0) {
        ActUserManager *manager(act_user_manager_get_default());

        if (manager != NULL) {
            gboolean loaded;
            g_object_get(manager, "is-loaded", &loaded, NULL);

            if (loaded) {
                const char *name(qPrintable(qgetenv("USER")));

                if (name != NULL && name[0] != '\0') {
                    ActUser *user(act_user_manager_get_user(manager, name));

                    if (user != NULL && act_user_is_loaded(user)) {
                        const char *language(act_user_get_language(user));
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

    return m_currentLanguage;
}

void
LanguagePlugin::userSetCurrentLanguage(ActUser *user)
{
    if (act_user_is_loaded(user)) {
        if (m_user != NULL) {
            g_signal_handlers_disconnect_by_data(m_user, this);
            g_object_unref(m_user);
            m_user = NULL;
        }

        if (m_nextCurrentLanguage != m_currentLanguage) {
            QString languageCode(languageCodes()[m_nextCurrentLanguage]);
            act_user_set_language(user, qPrintable(languageCode.left(languageCode.indexOf('.'))));
            act_user_set_formats_locale(user, qPrintable(languageCode));
            m_currentLanguage = m_nextCurrentLanguage;
            Q_EMIT currentLanguageChanged();
        }
    }
}

void
userSetCurrentLanguage(GObject    *object,
                       GParamSpec *pspec,
                       gpointer    user_data)
{
    Q_UNUSED(pspec);

    LanguagePlugin *plugin(static_cast<LanguagePlugin *>(user_data));
    plugin->userSetCurrentLanguage(ACT_USER(object));
}

void
LanguagePlugin::managerSetCurrentLanguage(ActUserManager *manager)
{
    gboolean loaded;
    g_object_get(manager, "is-loaded", &loaded, NULL);

    if (loaded) {
        if (m_manager != NULL) {
            g_signal_handlers_disconnect_by_data(m_manager, this);
            g_object_unref(m_manager);
            m_manager = NULL;
        }

        const char *name(qPrintable(qgetenv("USER")));

        if (name != NULL && name[0] != '\0') {
            ActUser *user(act_user_manager_get_user(manager, name));

            if (user != NULL) {
                if (act_user_is_loaded(user))
                    userSetCurrentLanguage(user);
                else {
                    m_user = static_cast<ActUser *>(g_object_ref(user));
                    g_signal_connect(user, "notify::is-loaded", G_CALLBACK(::userSetCurrentLanguage), this);
                }
            }
        }
    }
}

void
managerSetCurrentLanguage(GObject    *object,
                          GParamSpec *pspec,
                          gpointer    user_data)
{
    Q_UNUSED(pspec);

    LanguagePlugin *plugin(static_cast<LanguagePlugin *>(user_data));
    plugin->managerSetCurrentLanguage(ACT_USER_MANAGER(object));
}

void
LanguagePlugin::setCurrentLanguage(int index)
{
    if (index >= 0 && index < languageLocales().length()) {
        ActUserManager *manager(act_user_manager_get_default());

        if (manager != NULL) {
            m_nextCurrentLanguage = index;

            gboolean loaded;
            g_object_get(manager, "is-loaded", &loaded, NULL);

            if (loaded)
                managerSetCurrentLanguage(manager);
            else {
                m_manager = static_cast<ActUserManager *>(g_object_ref(manager));
                g_signal_connect(manager, "notify::is-loaded", G_CALLBACK(::managerSetCurrentLanguage), this);
            }
        }
    }
}

SubsetModel *
LanguagePlugin::keyboardLayoutsModel()
{
    if (m_keyboardLayoutsModel == NULL) {
        QVariantList superset;

        for (QList<KeyboardLayout *>::const_iterator i(keyboardLayouts().begin()); i != keyboardLayouts().end(); ++i) {
            QVariantList element;

            if (!(*i)->displayName().isEmpty())
                element += (*i)->displayName();
            else
                element += (*i)->name();

            element += (*i)->shortName();
            superset += QVariant(element);
        }

        GVariantIter *iter;
        const gchar *language;
        QList<int> subset;

        g_settings_get(maliitSettings(), KEY_ENABLED_LAYOUTS, "as", &iter);

        while (g_variant_iter_next(iter, "&s", &language)) {
            for (int i(0); i < keyboardLayouts().length(); i++) {
                if (keyboardLayouts()[i]->name() == language) {
                    subset += i;
                    break;
                }
            }
        }

        g_variant_iter_free(iter);

        QStringList customRoles;
        customRoles += "language";
        customRoles += "icon";

        m_keyboardLayoutsModel = new SubsetModel();
        m_keyboardLayoutsModel->setCustomRoles(customRoles);
        m_keyboardLayoutsModel->setSuperset(superset);
        m_keyboardLayoutsModel->setSubset(subset);
        m_keyboardLayoutsModel->setAllowEmpty(false);

        connect(m_keyboardLayoutsModel, SIGNAL(subsetChanged()), SLOT(updateKeyboardLayouts()));
    }

    return m_keyboardLayoutsModel;
}

void
LanguagePlugin::updateKeyboardLayouts()
{
    GVariantBuilder builder;
    GVariant *currentLayouts;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));

    for (QList<int>::const_iterator i(keyboardLayoutsModel()->subset().begin()); i != keyboardLayoutsModel()->subset().end(); ++i)
        g_variant_builder_add(&builder, "s", qPrintable(keyboardLayouts()[*i]->name()));

    currentLayouts = g_variant_ref_sink(g_variant_builder_end(&builder));
    g_settings_set_value(maliitSettings(), KEY_ENABLED_LAYOUTS, currentLayouts);
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
    if (m_spellCheckingModel == NULL) {
        // TODO: populate spell checking model
        QVariantList superset;

        for (QStringList::const_iterator i(languageNames().begin()); i != languageNames().end(); ++i) {
            QVariantList element;
            element += *i;
            superset += QVariant(element);
        }

        m_spellCheckingModel = new SubsetModel();
        m_spellCheckingModel->setCustomRoles(QStringList("language"));
        m_spellCheckingModel->setSuperset(superset);
        m_spellCheckingModel->setSubset(QList<int>());
        m_spellCheckingModel->setAllowEmpty(false);

        connect(m_spellCheckingModel, SIGNAL(subsetChanged()), SLOT(updateSpellChecking()));
    }

    return m_spellCheckingModel;
}

void
LanguagePlugin::updateSpellChecking()
{
    // TODO: update spell checking
}

bool
LanguagePlugin::autoCapitalization() const
{
    return g_settings_get_boolean(maliitSettings(), KEY_AUTO_CAPITALIZATION);
}

void
LanguagePlugin::setAutoCapitalization(bool value)
{
    if (value != autoCapitalization()) {
        g_settings_set_boolean(maliitSettings(), KEY_AUTO_CAPITALIZATION, value);
        Q_EMIT autoCapitalizationChanged();
    }
}

bool
LanguagePlugin::autoCompletion() const
{
    return g_settings_get_boolean(maliitSettings(), KEY_AUTO_COMPLETION);
}

void
LanguagePlugin::setAutoCompletion(bool value)
{
    if (value != autoCompletion()) {
        g_settings_set_boolean(maliitSettings(), KEY_AUTO_COMPLETION, value);
        Q_EMIT autoCompletionChanged();
    }
}

bool
LanguagePlugin::predictiveText() const
{
    return g_settings_get_boolean(maliitSettings(), KEY_PREDICTIVE_TEXT);
}

void
LanguagePlugin::setPredictiveText(bool value)
{
    if (value != predictiveText()) {
        g_settings_set_boolean(maliitSettings(), KEY_PREDICTIVE_TEXT, value);
        Q_EMIT predictiveTextChanged();
    }
}

bool
LanguagePlugin::keyPressFeedback() const
{
    return g_settings_get_boolean(maliitSettings(), KEY_KEY_PRESS_FEEDBACK);
}

void
LanguagePlugin::setKeyPressFeedback(bool value)
{
    if (value != keyPressFeedback()) {
        g_settings_set_boolean(maliitSettings(), KEY_KEY_PRESS_FEEDBACK, value);
        Q_EMIT keyPressFeedbackChanged();
    }
}

static bool
compareLocales(const QLocale &locale0,
               const QLocale &locale1)
{
    QString name0(locale0.nativeLanguageName().toCaseFolded());
    QString name1(locale1.nativeLanguageName().toCaseFolded());

    if (name0 == name1) {
        name0 = locale0.nativeCountryName().toCaseFolded();
        name1 = locale1.nativeCountryName().toCaseFolded();
    }

    return name0 < name1;
}

const QList<QLocale> &
LanguagePlugin::languageLocales() const
{
    if (m_languageLocales == NULL) {
        delete m_languageNames;
        delete m_languageCodes;
        delete m_indicesByBcp47Name;
        delete m_indicesByLocaleName;

        m_languageLocales = new QList<QLocale>;
        m_languageNames = new QStringList;
        m_languageCodes = new QStringList;
        m_indicesByBcp47Name = new QHash<QString, unsigned int>;
        m_indicesByLocaleName = new QHash<QString, unsigned int>;

        QProcess localeProcess;
        localeProcess.start("locale", QStringList("-a"), QIODevice::ReadOnly);
        localeProcess.waitForFinished();

        QString localeOutput(localeProcess.readAllStandardOutput());
        QStringList localeNames(localeOutput.split(QRegExp("\\s+")));
        QHash<QString, QString> localeNamesByBcp47Name;
        QHash<QString, unsigned int> countsByLanguage;

        for (QStringList::const_iterator i(localeNames.begin()); i != localeNames.end(); ++i) {
            QLocale locale(i->left(i->indexOf('.')));
            QString bcp47Name(locale.bcp47Name());
            QString language(locale.nativeLanguageName());

            if (!language.isEmpty() && !localeNamesByBcp47Name.contains(bcp47Name)) {
                *m_languageLocales += locale;
                localeNamesByBcp47Name[bcp47Name] = *i;
                countsByLanguage[language]++;
            }
        }

        qSort(m_languageLocales->begin(), m_languageLocales->end(), compareLocales);

        for (int i(0); i < m_languageLocales->length(); i++) {
            const QLocale &locale((*m_languageLocales)[i]);
            QString bcp47Name(locale.bcp47Name());
            QString language(locale.nativeLanguageName());
            unsigned int count(countsByLanguage[language]);

            if (count > 1)
                *m_languageNames += QString("%1 - %2").arg(language).arg(locale.nativeCountryName());
            else
                *m_languageNames += language;

            *m_languageCodes += localeNamesByBcp47Name[bcp47Name];
            (*m_indicesByBcp47Name)[bcp47Name] = i;
            (*m_indicesByLocaleName)[localeNamesByBcp47Name[bcp47Name]] = i;
        }
    }

    return *m_languageLocales;
}

int
LanguagePlugin::indexForLocale(const QLocale &locale) const
{
    languageLocales();
    return m_indicesByBcp47Name->value(locale.bcp47Name(), -1);
}

int
LanguagePlugin::indexForLanguage(const QString &language) const
{
    languageLocales();
    return m_indicesByLocaleName->value(language, -1);
}

GSettings *
LanguagePlugin::maliitSettings() const
{
    if (m_maliitSettings == NULL)
        m_maliitSettings = g_settings_new(UBUNTU_KEYBOARD_SCHEMA_ID);

    return m_maliitSettings;
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

const QList<KeyboardLayout *> &
LanguagePlugin::keyboardLayouts() const
{
    if (m_keyboardLayouts == NULL) {
        m_keyboardLayouts = new QList<KeyboardLayout *>;

        QDir layoutsDir(LAYOUTS_DIR);
        layoutsDir.setFilter(QDir::Files);
        layoutsDir.setNameFilters(QStringList("*.xml"));
        layoutsDir.setSorting(QDir::Name);

        QFileInfoList fileInfoList(layoutsDir.entryInfoList());

        for (QFileInfoList::const_iterator i(fileInfoList.begin()); i != fileInfoList.end(); ++i) {
            KeyboardLayout *layout(new KeyboardLayout(*i));

            if (!layout->language().isEmpty())
                *m_keyboardLayouts += layout;
            else
                delete layout;
        }

        qSort(m_keyboardLayouts->begin(), m_keyboardLayouts->end(), compareLayouts);
    }

    return *m_keyboardLayouts;
}
