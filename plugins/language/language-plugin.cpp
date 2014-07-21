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
#include "keyboard-layout.h"

#include <act/act.h>
#include <unicode/locid.h>

#define UBUNTU_KEYBOARD_SCHEMA_ID "com.canonical.keyboard.maliit"

#define KEY_ENABLED_LAYOUTS     "enabled-languages"
#define KEY_CURRENT_LAYOUT      "active-language"
#define KEY_SPELL_CHECKING      "spell-checking"
#define KEY_AUTO_CAPITALIZATION "auto-capitalization"
#define KEY_AUTO_COMPLETION     "auto-completion"
#define KEY_PREDICTIVE_TEXT     "predictive-text"
#define KEY_KEY_PRESS_FEEDBACK  "key-press-feedback"

#define LANGUAGE2LOCALE "/usr/share/language-tools/language2locale"
#define LAYOUTS_DIR "/usr/share/maliit/plugins/com/ubuntu/lib"

static const char * const LOCALE_BLACKLIST[] = {
    "C",
    "C.UTF-8",
    "POSIX",
    NULL
};

struct LanguageLocale
{
public:

    // Should be true if locale is the default for its language.
    // e.g. 'en_US' is the likely locale for 'en', 'en_CA' is not.
    bool likely;

    QString localeName;
    QString displayName;
    icu::Locale locale;

public:

    explicit LanguageLocale(const QString &name);

    bool operator<(const LanguageLocale &l) const;
};

LanguageLocale::LanguageLocale(const QString &name) :
    likely(false),
    localeName(name),
    locale(qPrintable(name))
{
    std::string string;
    icu::UnicodeString unicodeString;
    locale.getDisplayName(locale, unicodeString);
    unicodeString.toUTF8String(string);
    displayName = string.c_str();
}

bool LanguageLocale::operator<(const LanguageLocale &l) const
{
    // Likely locales should precede unlikely ones of the same language.
    if (strcasecmp(locale.getLanguage(), l.locale.getLanguage()) == 0) {
        if (likely || l.likely)
            return likely && !l.likely;
    }

    return displayName.compare(l.displayName, Qt::CaseInsensitive) < 0;
}

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
            g_signal_connect(m_manager, "notify::is-loaded",
                             G_CALLBACK(::managerLoaded), this);
    }

    updateLanguageNamesAndCodes();
    updateCurrentLanguage();
    updateEnabledLayouts();
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

    for (QList<KeyboardLayout *>::const_iterator
         i(m_keyboardLayouts.begin()); i != m_keyboardLayouts.end(); ++i)
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
    if (index >= 0 && index < m_languageCodes.length()) {
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
    gchar *current;
    bool removed(true);

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    g_settings_get(m_maliitSettings, KEY_CURRENT_LAYOUT, "s", &current);

    for (QList<int>::const_iterator
         i(m_keyboardLayoutsModel.subset().begin());
         i != m_keyboardLayoutsModel.subset().end(); ++i) {
        g_variant_builder_add(&builder, "s",
                              qPrintable(m_keyboardLayouts[*i]->name()));

        if (m_keyboardLayouts[*i]->name() == current)
            removed = false;
    }

    if (removed && !m_keyboardLayoutsModel.subset().isEmpty()) {
        GVariantIter *iter;
        const gchar *layout;
        bool found(false);

        g_settings_get(m_maliitSettings, KEY_ENABLED_LAYOUTS, "as", &iter);

        for (int i(0); g_variant_iter_next(iter, "&s", &layout); i++) {
            found = g_strcmp0(layout, current) == 0;

            if (found) {
                if (i >= m_keyboardLayoutsModel.subset().size())
                    i = m_keyboardLayoutsModel.subset().size() - 1;

                int index(m_keyboardLayoutsModel.subset()[i]);
                const QString &name(m_keyboardLayouts[index]->name());

                g_settings_set_string(m_maliitSettings,
                                      KEY_CURRENT_LAYOUT, qPrintable(name));

                break;
            }
        }

        if (!found) {
            int index(m_keyboardLayoutsModel.subset().front());
            const QString &name(m_keyboardLayouts[index]->name());

            g_settings_set_string(m_maliitSettings,
                                  KEY_CURRENT_LAYOUT, qPrintable(name));
        }

        g_variant_iter_free(iter);
    }

    g_free(current);
    g_settings_set_value(m_maliitSettings,
                         KEY_ENABLED_LAYOUTS, g_variant_builder_end(&builder));
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
LanguagePlugin::spellChecking() const
{
    return g_settings_get_boolean(m_maliitSettings, KEY_SPELL_CHECKING);
}

void
LanguagePlugin::setSpellChecking(bool value)
{
    if (value != spellChecking()) {
        g_settings_set_boolean(m_maliitSettings,
                               KEY_SPELL_CHECKING, value);
        Q_EMIT spellCheckingChanged();
    }
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
        g_settings_set_boolean(m_maliitSettings,
                               KEY_AUTO_CAPITALIZATION, value);
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
LanguagePlugin::updateLanguageNamesAndCodes()
{
    m_languageNames.clear();
    m_languageCodes.clear();
    m_indicesByLocale.clear();

    // Get locales from 'locale -a'.
    QProcess localeProcess;
    localeProcess.start("locale", QStringList("-a"), QIODevice::ReadOnly);
    localeProcess.waitForFinished();
    QString localeOutput(localeProcess.readAllStandardOutput());
    QSet<QString> localeNames(localeOutput.split(QRegExp("\\s+")).toSet());

    QHash<QString, QString> likelyLocaleForLanguage;
    QList<LanguageLocale> languageLocales;

    // Remove blacklisted locales.
    for (unsigned int
         i(0); i < sizeof(LOCALE_BLACKLIST) / sizeof(const char *); i++)
        localeNames.remove(LOCALE_BLACKLIST[i]);

    for (QSet<QString>::const_iterator
         i(localeNames.begin()); i != localeNames.end(); ++i) {
        // We only want locales that contain '.utf8'.
        if (i->indexOf(".utf8") < 0)
            continue;

        LanguageLocale languageLocale(*i);

        // Filter out locales for which we have no display name.
        if (languageLocale.displayName.isEmpty())
            continue;

        QString language(languageLocale.locale.getLanguage());

        if (!likelyLocaleForLanguage.contains(language)) {
            QProcess likelyProcess;
            likelyProcess.start(LANGUAGE2LOCALE,
                                QStringList(language),
                                QIODevice::ReadOnly);
            likelyProcess.waitForFinished();
            QString likelyLocale(likelyProcess.readAllStandardOutput());
            likelyLocale = likelyLocale.left(likelyLocale.indexOf('.'));
            likelyLocaleForLanguage.insert(language, likelyLocale.trimmed());
        }

        languageLocale.likely = likelyLocaleForLanguage[language] ==
                                i->left(i->indexOf('.'));
        languageLocales += languageLocale;
    }

    qSort(languageLocales);

    for (int i(0); i < languageLocales.length(); i++) {
        const LanguageLocale &languageLocale(languageLocales[i]);

        m_languageNames += languageLocale.displayName;
        m_languageCodes += languageLocale.localeName;

        QString localeName(languageLocale.localeName);
        localeName = localeName.left(localeName.indexOf('.'));
        m_indicesByLocale.insert(localeName, i);

        if (languageLocale.likely) {
            localeName = localeName.left(localeName.indexOf('_'));
            m_indicesByLocale.insert(localeName, i);
        }
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

            QString formatsLocale(m_languageCodes[m_currentLanguage]);
            QString language(formatsLocale.left(formatsLocale.indexOf('.')));
            act_user_set_language(m_user, qPrintable(language));
            act_user_set_formats_locale(m_user, qPrintable(formatsLocale));

            icu::Locale locale(qPrintable(formatsLocale));
            const char *code(locale.getLanguage());
            QFileInfo fileInfo(QDir(LAYOUTS_DIR), code);

            if (fileInfo.exists() && fileInfo.isDir()) {
                g_settings_set_string(m_maliitSettings,
                                      KEY_CURRENT_LAYOUT, code);

                updateEnabledLayouts();
            }
        } else {
            QString formatsLocale(act_user_get_formats_locale(m_user));
            m_currentLanguage = indexForLocale(formatsLocale);

            if (m_currentLanguage < 0) {
                QString language(act_user_get_language(m_user));
                m_currentLanguage = indexForLocale(language);
            }
        }
    }

    if (m_currentLanguage < 0)
        m_currentLanguage = indexForLocale(QLocale::system().name());

    if (m_currentLanguage != previousLanguage)
        Q_EMIT currentLanguageChanged();
}

void
LanguagePlugin::updateEnabledLayouts()
{
    GVariantBuilder builder;
    GVariantIter *iter;
    gchar *current;
    const gchar *layout;
    QSet<QString> added;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    g_settings_get(m_maliitSettings, KEY_ENABLED_LAYOUTS, "as", &iter);
    g_settings_get(m_maliitSettings, KEY_CURRENT_LAYOUT, "s", &current);

    while (g_variant_iter_next(iter, "&s", &layout)) {
        if (!added.contains(layout)) {
            g_variant_builder_add(&builder, "s", layout);
            added.insert(layout);
        }
    }

    if (!added.contains(current)) {
        g_variant_builder_add(&builder, "s", current);
        added.insert(current);
    }

    g_free(current);
    g_variant_iter_free(iter);
    g_settings_set_value(m_maliitSettings,
                         KEY_ENABLED_LAYOUTS, g_variant_builder_end(&builder));
}

void
LanguagePlugin::updateKeyboardLayouts()
{
    m_keyboardLayouts.clear();

    QDir layoutsDir(LAYOUTS_DIR);
    layoutsDir.setFilter(QDir::Dirs);
    layoutsDir.setSorting(QDir::Name);

    QFileInfoList fileInfoList(layoutsDir.entryInfoList());

    for (QFileInfoList::const_iterator
         i(fileInfoList.begin()); i != fileInfoList.end(); ++i) {
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

    for (QList<KeyboardLayout *>::const_iterator
         i(m_keyboardLayouts.begin()); i != m_keyboardLayouts.end(); ++i) {
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

    connect(&m_keyboardLayoutsModel,
            SIGNAL(subsetChanged()), SLOT(keyboardLayoutsModelChanged()));

    g_signal_connect(m_maliitSettings, "changed::" KEY_ENABLED_LAYOUTS,
                     G_CALLBACK(::enabledLayoutsChanged), this);
}

void
LanguagePlugin::updateSpellCheckingModel()
{
    // TODO: populate spell checking model
    QVariantList superset;

    for (QStringList::const_iterator
         i(m_languageNames.begin()); i != m_languageNames.end(); ++i) {
        QVariantList element;
        element += *i;
        superset += QVariant(element);
    }

    m_spellCheckingModel.setCustomRoles(QStringList("language"));
    m_spellCheckingModel.setSuperset(superset);
    m_spellCheckingModel.setSubset(QList<int>());
    m_spellCheckingModel.setAllowEmpty(false);

    connect(&m_spellCheckingModel,
            SIGNAL(subsetChanged()), SLOT(spellCheckingModelChanged()));
}

int
LanguagePlugin::indexForLocale(const QString &name)
{
    return m_indicesByLocale.value(name.left(name.indexOf('.')), -1);
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
                    g_signal_connect(m_user, "notify::is-loaded",
                                     G_CALLBACK(::userLoaded), this);
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
    const gchar *layout;
    QList<int> subset;

    g_settings_get(m_maliitSettings, KEY_ENABLED_LAYOUTS, "as", &iter);

    while (g_variant_iter_next(iter, "&s", &layout)) {
        for (int i(0); i < m_keyboardLayouts.length(); i++) {
            if (m_keyboardLayouts[i]->name() == layout) {
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
