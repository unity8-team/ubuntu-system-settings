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

    Q_PROPERTY(QStringList plugins
               READ plugins
               CONSTANT)

    Q_PROPERTY(QList<int> currentPlugins
               READ currentPlugins
               WRITE setCurrentPlugins
               NOTIFY currentPluginsChanged)

    Q_PROPERTY(bool autoCapitalization
               READ autoCapitalization
               WRITE setAutoCapitalization
               NOTIFY autoCapitalizationChanged)

    Q_PROPERTY(bool autoCompletion
               READ autoCompletion
               WRITE setAutoCompletion
               NOTIFY autoCompletionChanged)

    Q_PROPERTY(bool autoCorrection
               READ autoCorrection
               WRITE setAutoCorrection
               NOTIFY autoCorrectionChanged)

    Q_PROPERTY(bool autoPunctuation
               READ autoPunctuation
               WRITE setAutoPunctuation
               NOTIFY autoPunctuationChanged)

public:

    explicit LanguagePlugin(QObject *parent = NULL);

    const QStringList &languages() const;

    int currentLanguage() const;
    void setCurrentLanguage(int index);
    Q_SIGNAL void currentLanguageChanged();

    const QStringList &plugins() const;

    const QList<int> &currentPlugins() const;
    void setCurrentPlugins(const QList<int> &list);
    Q_INVOKABLE bool isCurrentPlugin(int index) const;
    Q_SIGNAL void currentPluginsChanged();

    bool autoCapitalization() const;
    void setAutoCapitalization(bool value);
    Q_SIGNAL void autoCapitalizationChanged();

    bool autoCompletion() const;
    void setAutoCompletion(bool value);
    Q_SIGNAL void autoCompletionChanged();

    bool autoCorrection() const;
    void setAutoCorrection(bool value);
    Q_SIGNAL void autoCorrectionChanged();

    bool autoPunctuation() const;
    void setAutoPunctuation(bool value);
    Q_SIGNAL void autoPunctuationChanged();
};

#endif // LANGUAGE_PLUGIN_H
