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
    _languageLocales(NULL),
    _languageNames(NULL),
    _languageCodes(NULL),
    _nameIndices(NULL),
    _codeIndices(NULL),
    _currentLanguage(-1),
    _nextCurrentLanguage(-1),
    _manager(NULL),
    _user(NULL),
    _maliitSettings(NULL),
    _keyboardLayouts(NULL),
    _keyboardLayoutsModel(NULL),
    _spellCheckingModel(NULL)
{
}

LanguagePlugin::~LanguagePlugin()
{
    delete _languageLocales;
    delete _languageNames;
    delete _languageCodes;
    delete _nameIndices;
    delete _codeIndices;

    if (_manager != NULL) {
        g_signal_handlers_disconnect_by_data(_manager, this);
        g_object_unref(_manager);
    }

    if (_user != NULL) {
        g_signal_handlers_disconnect_by_data(_user, this);
        g_object_unref(_user);
    }

    if (_maliitSettings != NULL)
        g_object_unref(_maliitSettings);

    for (QList<KeyboardLayout *>::const_iterator i(_keyboardLayouts->begin()); i != _keyboardLayouts->end(); ++i)
        delete *i;

    delete _keyboardLayouts;
    delete _keyboardLayoutsModel;
    delete _spellCheckingModel;
}

const QStringList &
LanguagePlugin::languageNames() const
{
    languageLocales();
    return *_languageNames;
}

const QStringList &
LanguagePlugin::languageCodes() const
{
    languageLocales();
    return *_languageCodes;
}

int
LanguagePlugin::currentLanguage() const
{
    if (_currentLanguage < 0) {
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
                        QHash<QString, unsigned int>::const_iterator i(codeIndices().find(language));

                        if (i == codeIndices().end()) {
                            QLocale locale(language);
                            i = nameIndices().find(locale.nativeLanguageName().trimmed());

                            if (i == nameIndices().end()) {
                                locale = QLocale(locale.language());
                                i = nameIndices().find(locale.nativeLanguageName().trimmed());
                            }
                        }

                        if (i != codeIndices().end() && i != nameIndices().end())
                            _currentLanguage = *i;
                    }
                }
            }
        }
    }

    if (_currentLanguage < 0) {
        QLocale locale(QLocale::system());
        QHash<QString, unsigned int>::const_iterator i(codeIndices().find(locale.name().trimmed()));

        if (i == codeIndices().end()) {
            locale = QLocale(locale.language());
            i = codeIndices().find(locale.name().trimmed());
        }

        if (i != codeIndices().end())
            _currentLanguage = *i;
    }

    return _currentLanguage;
}

void
LanguagePlugin::userSetCurrentLanguage(ActUser *user)
{
    if (act_user_is_loaded(user)) {
        if (_user != NULL) {
            g_signal_handlers_disconnect_by_data(_user, this);
            g_object_unref(_user);
            _user = NULL;
        }

        if (_nextCurrentLanguage != _currentLanguage) {
            act_user_set_language(user, qPrintable(languageCodes()[_nextCurrentLanguage]));
            _currentLanguage = _nextCurrentLanguage;
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
        if (_manager != NULL) {
            g_signal_handlers_disconnect_by_data(_manager, this);
            g_object_unref(_manager);
            _manager = NULL;
        }

        const char *name(qPrintable(qgetenv("USER")));

        if (name != NULL && name[0] != '\0') {
            ActUser *user(act_user_manager_get_user(manager, name));

            if (user != NULL) {
                if (act_user_is_loaded(user))
                    userSetCurrentLanguage(user);
                else {
                    _user = static_cast<ActUser *>(g_object_ref(user));
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
            _nextCurrentLanguage = index;

            gboolean loaded;
            g_object_get(manager, "is-loaded", &loaded, NULL);

            if (loaded)
                managerSetCurrentLanguage(manager);
            else {
                _manager = static_cast<ActUserManager *>(g_object_ref(manager));
                g_signal_connect(manager, "notify::is-loaded", G_CALLBACK(::managerSetCurrentLanguage), this);
            }
        }
    }
}

SubsetModel *
LanguagePlugin::keyboardLayoutsModel()
{
    if (_keyboardLayoutsModel == NULL) {
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

        _keyboardLayoutsModel = new SubsetModel();
        _keyboardLayoutsModel->setCustomRoles(customRoles);
        _keyboardLayoutsModel->setSuperset(superset);
        _keyboardLayoutsModel->setSubset(subset);
        _keyboardLayoutsModel->setAllowEmpty(false);

        connect(_keyboardLayoutsModel, SIGNAL(subsetChanged()), SLOT(updateKeyboardLayouts()));
    }

    return _keyboardLayoutsModel;
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
    if (_spellCheckingModel == NULL) {
        // TODO: populate spell checking model
        QVariantList superset;

        for (QStringList::const_iterator i(languageNames().begin()); i != languageNames().end(); ++i) {
            QVariantList element;
            element += *i;
            superset += QVariant(element);
        }

        _spellCheckingModel = new SubsetModel();
        _spellCheckingModel->setCustomRoles(QStringList("language"));
        _spellCheckingModel->setSuperset(superset);
        _spellCheckingModel->setSubset(QList<int>());
        _spellCheckingModel->setAllowEmpty(false);

        connect(_spellCheckingModel, SIGNAL(subsetChanged()), SLOT(updateSpellChecking()));
    }

    return _spellCheckingModel;
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
    QString name0(locale0.nativeLanguageName().trimmed().toCaseFolded());
    QString name1(locale1.nativeLanguageName().trimmed().toCaseFolded());

    return name0 < name1;
}

const QList<QLocale> &
LanguagePlugin::languageLocales() const
{
    if (_languageLocales == NULL) {
        delete _languageNames;
        delete _languageCodes;
        delete _nameIndices;
        delete _codeIndices;

        _languageLocales = new QList<QLocale>;
        _languageNames = new QStringList;
        _languageCodes = new QStringList;
        _nameIndices = new QHash<QString, unsigned int>;
        _codeIndices = new QHash<QString, unsigned int>;

        QProcess localeProcess;
        localeProcess.start("locale", QStringList("-a"), QIODevice::ReadOnly);
        localeProcess.waitForFinished();

        QString localeOutput(localeProcess.readAllStandardOutput());
        QStringList localeNames(localeOutput.split(QRegExp("\\s+")));
        QSet<QString> languageNames;

        for (QStringList::const_iterator i(localeNames.begin()); i != localeNames.end(); ++i) {
            QString localeName(i->left(i->indexOf('.')));
            QLocale locale(localeName);
            QString languageName(locale.nativeLanguageName().trimmed().toCaseFolded());

            if (!languageName.isEmpty()) {
                QLocale genericLocale(locale.language());
                QString genericName(genericLocale.nativeLanguageName().trimmed().toCaseFolded());

                if (genericName == languageName) {
                    if (!languageNames.contains(genericName)) {
                        *_languageLocales += genericLocale;
                        languageNames += genericName;
                    }
                } else {
                    if (!languageNames.contains(languageName)) {
                        *_languageLocales += locale;
                        languageNames += languageName;
                    }
                }
            }
        }

        qSort(_languageLocales->begin(), _languageLocales->end(), compareLocales);

        for (int i(0); i < _languageLocales->length(); i++) {
            *_languageNames += (*_languageLocales)[i].nativeLanguageName().trimmed();
            *_languageCodes += (*_languageLocales)[i].name().trimmed();
            (*_nameIndices)[(*_languageNames)[i]] = i;
            (*_codeIndices)[(*_languageCodes)[i]] = i;
        }
    }

    return *_languageLocales;
}

const QHash<QString, unsigned int> &
LanguagePlugin::nameIndices() const
{
    languageLocales();
    return *_nameIndices;
}

const QHash<QString, unsigned int> &
LanguagePlugin::codeIndices() const
{
    languageLocales();
    return *_codeIndices;
}

GSettings *
LanguagePlugin::maliitSettings() const
{
    if (_maliitSettings == NULL)
        _maliitSettings = g_settings_new(UBUNTU_KEYBOARD_SCHEMA_ID);

    return _maliitSettings;
}

static bool
compareLayouts(const KeyboardLayout *layout0,
               const KeyboardLayout *layout1)
{
    const QString &displayName0(layout0->displayName());
    const QString &displayName1(layout1->displayName());
    const QString &language0(layout0->language());
    const QString &language1(layout1->language());
    const QString &name0(layout0->name());
    const QString &name1(layout1->name());

    return displayName0 < displayName1 || (displayName0 == displayName1 && (language0 < language1 || (language0 == language1 && name0 < name1)));
}

const QList<KeyboardLayout *> &
LanguagePlugin::keyboardLayouts() const
{
    if (_keyboardLayouts == NULL) {
        _keyboardLayouts = new QList<KeyboardLayout *>;

        QDir layoutsDir(LAYOUTS_DIR);
        layoutsDir.setFilter(QDir::Files);
        layoutsDir.setNameFilters(QStringList("*.xml"));
        layoutsDir.setSorting(QDir::Name);

        QFileInfoList fileInfoList(layoutsDir.entryInfoList());

        for (QFileInfoList::const_iterator i(fileInfoList.begin()); i != fileInfoList.end(); ++i) {
            KeyboardLayout *layout(new KeyboardLayout(*i));

            if (!layout->language().isEmpty())
                *_keyboardLayouts += layout;
            else
                delete layout;
        }

        qSort(_keyboardLayouts->begin(), _keyboardLayouts->end(), compareLayouts);
    }

    return *_keyboardLayouts;
}
