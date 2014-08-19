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

    Q_PROPERTY(SubsetModel *spellCheckingModel
               READ spellCheckingModel
               CONSTANT)

    explicit LanguagePlugin(QObject *parent = NULL);

    virtual ~LanguagePlugin();

    const QStringList &languageNames() const;
    const QStringList &languageCodes() const;

    int currentLanguage() const;
    void setCurrentLanguage(int index);
    Q_SIGNAL void currentLanguageChanged() const;

    SubsetModel *keyboardLayoutsModel();
    Q_SLOT void keyboardLayoutsModelChanged();

    SubsetModel *spellCheckingModel();
    Q_SLOT void spellCheckingModelChanged();

private:

    void updateLanguageNamesAndCodes();
    void updateCurrentLanguage();
    void updateEnabledLayouts();
    void updateKeyboardLayouts();
    void updateKeyboardLayoutsModel();
    void updateSpellCheckingModel();

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
    SubsetModel m_spellCheckingModel;
};

#endif // LANGUAGE_PLUGIN_H
