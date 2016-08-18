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
#include <QtDebug>
#include "language-plugin.h"
#include "keyboard-layout.h"

#include <act/act.h>
#include <unicode/locid.h>

#define LANGUAGE2LOCALE "/usr/share/language-tools/language2locale"

static const char * const LOCALE_BLACKLIST[] = {
    "C",
    "C.UTF-8",
    "POSIX",
    nullptr
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
    /* workaround iso-codes casing being inconsistant */
    if (displayName.length() > 0)
        displayName[0] = displayName[0].toUpper();
}

bool LanguageLocale::operator<(const LanguageLocale &l) const
{
    // Likely locales should precede unlikely ones of the same language.
    if (strcasecmp(locale.getLanguage(), l.locale.getLanguage()) == 0) {
        if (likely || l.likely)
            return likely && !l.likely;
    }

    return QString::localeAwareCompare(displayName, l.displayName) < 0;
}

void managerLoaded(GObject    *object,
                   GParamSpec *pspec,
                   gpointer    user_data);

LanguagePlugin::LanguagePlugin(QObject *parent) :
    QObject(parent),
    m_currentLanguage(-1),
    m_nextCurrentLanguage(-1),
    m_manager(act_user_manager_get_default()),
    m_user(nullptr)
{
    if (m_manager != nullptr) {
        g_object_ref(m_manager);

        gboolean loaded;
        g_object_get(m_manager, "is-loaded", &loaded, nullptr);

        if (loaded)
            managerLoaded();
        else
            g_signal_connect(m_manager, "notify::is-loaded",
                             G_CALLBACK(::managerLoaded), this);
    }

    updateLanguageNamesAndCodes();
    updateCurrentLanguage();
    updateSpellCheckingModel();
}

LanguagePlugin::~LanguagePlugin()
{
    if (m_user != nullptr) {
        g_signal_handlers_disconnect_by_data(m_user, this);
        g_object_unref(m_user);
    }

    if (m_manager != nullptr) {
        g_signal_handlers_disconnect_by_data(m_manager, this);
        g_object_unref(m_manager);
    }
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

QString
LanguagePlugin::languageToLayout(const QString &lang)
{
    QString language(lang.left(lang.indexOf('.')));
    act_user_set_language(m_user, qPrintable(language));
    act_user_set_formats_locale(m_user, qPrintable(lang));

    icu::Locale locale(qPrintable(lang));
    return locale.getLanguage();
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
LanguagePlugin::spellCheckingModel()
{
    return &m_spellCheckingModel;
}

void
LanguagePlugin::spellCheckingModelChanged()
{
    // TODO: update spell checking model
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

    if (m_user != nullptr && act_user_is_loaded(m_user)) {
        if (m_nextCurrentLanguage >= 0) {
            m_currentLanguage = m_nextCurrentLanguage;
            m_nextCurrentLanguage = -1;

            QString formatsLocale(m_languageCodes[m_currentLanguage]);
            QString language(formatsLocale.left(formatsLocale.indexOf('.')));
            act_user_set_language(m_user, qPrintable(language));
            act_user_set_formats_locale(m_user, qPrintable(formatsLocale));
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
    g_object_get(m_manager, "is-loaded", &loaded, nullptr);

    if (loaded) {
        g_signal_handlers_disconnect_by_data(m_manager, this);

        m_user = act_user_manager_get_user_by_id(m_manager, geteuid());

        if (m_user != nullptr) {
            g_object_ref(m_user);

            if (act_user_is_loaded(m_user))
                userLoaded();
            else
                g_signal_connect(m_user, "notify::is-loaded",
                                 G_CALLBACK(::userLoaded), this);
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

void LanguagePlugin::reboot()
{
    m_sessionService.reboot();
}
