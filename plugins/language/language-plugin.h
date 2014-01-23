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

#ifndef LANGUAGE_PLUGIN_H
#define LANGUAGE_PLUGIN_H

#include <QtCore>
#include "subset-model.h"

typedef struct _ActUser ActUser;
typedef struct _ActUserManager ActUserManager;
typedef struct _GObject GObject;
typedef struct _GParamSpec GParamSpec;
typedef struct _GSettings GSettings;
typedef void *gpointer;
typedef char gchar;

class KeyboardLayout;

class LanguagePlugin : public QObject
{
private:

    Q_OBJECT

public:

    Q_PROPERTY(QStringList languageNames
               READ languageNames
               CONSTANT)

    Q_PROPERTY(QStringList languageCodes
               READ languageCodes
               CONSTANT)

    Q_PROPERTY(int currentLanguage
               READ currentLanguage
               WRITE setCurrentLanguage
               NOTIFY currentLanguageChanged)

    Q_PROPERTY(SubsetModel *keyboardLayoutsModel
               READ keyboardLayoutsModel
               CONSTANT)

    Q_PROPERTY(bool spellChecking
               READ spellChecking
               WRITE setSpellChecking
               NOTIFY spellCheckingChanged)

    Q_PROPERTY(bool autoCapitalization
               READ autoCapitalization
               WRITE setAutoCapitalization
               NOTIFY autoCapitalizationChanged)

    Q_PROPERTY(bool autoCompletion
               READ autoCompletion
               WRITE setAutoCompletion
               NOTIFY autoCompletionChanged)

    Q_PROPERTY(bool predictiveText
               READ predictiveText
               WRITE setPredictiveText
               NOTIFY predictiveTextChanged)

    Q_PROPERTY(bool keyPressFeedback
               READ keyPressFeedback
               WRITE setKeyPressFeedback
               NOTIFY keyPressFeedbackChanged)

    explicit LanguagePlugin(QObject *parent = NULL);

    virtual ~LanguagePlugin();

    const QStringList &languageNames() const;
    const QStringList &languageCodes() const;

    int currentLanguage() const;
    void setCurrentLanguage(int index);
    Q_SIGNAL void currentLanguageChanged() const;

    SubsetModel *keyboardLayoutsModel();
    Q_SLOT void keyboardLayoutsModelChanged();

    bool spellChecking() const;
    void setSpellChecking(bool value);
    Q_SIGNAL void spellCheckingChanged() const;

    bool autoCapitalization() const;
    void setAutoCapitalization(bool value);
    Q_SIGNAL void autoCapitalizationChanged() const;

    bool autoCompletion() const;
    void setAutoCompletion(bool value);
    Q_SIGNAL void autoCompletionChanged() const;

    bool predictiveText() const;
    void setPredictiveText(bool value);
    Q_SIGNAL void predictiveTextChanged() const;

    bool keyPressFeedback() const;
    void setKeyPressFeedback(bool value);
    Q_SIGNAL void keyPressFeedbackChanged() const;

private:

    void updateLanguageNamesAndCodes();
    void updateCurrentLanguage();
    void updateKeyboardLayouts();
    void updateKeyboardLayoutsModel();

    int indexForLocale(const QString &name);

    void userLoaded();

    friend void userLoaded(GObject    *object,
                           GParamSpec *pspec,
                           gpointer    user_data);

    void managerLoaded();

    friend void managerLoaded(GObject    *object,
                              GParamSpec *pspec,
                              gpointer    user_data);

    void enabledLayoutsChanged();

    friend void enabledLayoutsChanged(GSettings *settings,
                                      gchar     *key,
                                      gpointer   user_data);

    QStringList m_languageNames;
    QStringList m_languageCodes;
    QHash<QString, unsigned int> m_indicesByLocale;

    int m_currentLanguage;
    int m_nextCurrentLanguage;
    ActUserManager *m_manager;
    ActUser *m_user;

    GSettings *m_maliitSettings;
    QList<KeyboardLayout *> m_keyboardLayouts;
    SubsetModel m_keyboardLayoutsModel;
};

#endif // LANGUAGE_PLUGIN_H
