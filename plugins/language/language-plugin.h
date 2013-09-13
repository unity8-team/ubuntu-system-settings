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

class LanguagePlugin : public QObject
{
private:

    Q_OBJECT

    Q_PROPERTY(QStringList languages
               READ languages
               CONSTANT)

    Q_PROPERTY(int currentLanguage
               READ currentLanguage
               WRITE setCurrentLanguage
               NOTIFY currentLanguageChanged)

    Q_PROPERTY(SubsetModel *layoutsModel
               READ layoutsModel
               CONSTANT)

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

public:

    explicit LanguagePlugin(QObject *parent = NULL);

    const QStringList &languages() const;

    int currentLanguage() const;
    void setCurrentLanguage(int index);
    Q_SIGNAL void currentLanguageChanged() const;

    SubsetModel *layoutsModel();
    Q_SLOT void updateLayouts();

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

    bool _updateLayoutsConnected;
};

#endif // LANGUAGE_PLUGIN_H
