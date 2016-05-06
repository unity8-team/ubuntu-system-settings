/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
 *          Jonas G. Drange <jonas.drange@canonical.com>
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

#ifndef OSK_PLUGIN_H
#define OSK_PLUGIN_H

#include <QtCore>
#include <gio/gio.h>
#include "subset-model.h"
#include "keyboard-layout.h"

typedef struct _GSettings GSettings;
typedef void *gpointer;
typedef char gchar;

class KeyboardLayout;

class OnScreenKeyboardPlugin : public QObject
{
private:

    Q_OBJECT

public:

    Q_PROPERTY(SubsetModel *keyboardLayoutsModel
               READ keyboardLayoutsModel
               CONSTANT)

    explicit OnScreenKeyboardPlugin(QObject *parent = nullptr);

    virtual ~OnScreenKeyboardPlugin();

    Q_INVOKABLE void setCurrentLayout(const QString &code);

    SubsetModel *keyboardLayoutsModel();
    Q_SLOT void keyboardLayoutsModelChanged();

private:

    void updateEnabledLayouts();
    void updateKeyboardLayouts();
    void updateKeyboardLayoutsModel();

    void enabledLayoutsChanged();

    friend void enabledLayoutsChanged(GSettings *settings,
                                      gchar     *key,
                                      gpointer   user_data);

    GSettings *m_maliitSettings;
    QList<KeyboardLayout *> m_keyboardLayouts;
    SubsetModel m_keyboardLayoutsModel;
    QStringList m_layoutPaths;
};

#endif // OSK_PLUGIN_H
