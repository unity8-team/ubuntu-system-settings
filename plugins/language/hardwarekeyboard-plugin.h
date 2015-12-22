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

#ifndef HWKBD_PLUGIN_H
#define HWKBD_PLUGIN_H

#include <QtCore>
#include <gio/gio.h>
#include <QDBusArgument>

#define GNOME_DESKTOP_USE_UNSTABLE_API
#include <libgnome-desktop/gnome-xkb-info.h>

#include "accountsservice.h"
#include "subset-model.h"
#include "keyboard-layout.h"

typedef void *gpointer;
typedef char gchar;

class KeyboardLayout;

class HardwareKeyboardPlugin : public QObject
{
private:

    Q_OBJECT

public:

    Q_PROPERTY(SubsetModel *keyboardLayoutsModel
               READ keyboardLayoutsModel
               CONSTANT)

    explicit HardwareKeyboardPlugin(QObject *parent = nullptr);

    virtual ~HardwareKeyboardPlugin();

    Q_INVOKABLE void setCurrentLayout(const QString &code);

    SubsetModel *keyboardLayoutsModel();
    Q_SLOT void keyboardLayoutsModelChanged();
    Q_SLOT void enabledLayoutsChanged();

private:
    void updateEnabledLayouts();
    void updateKeyboardLayouts();
    void updateKeyboardLayoutsModel();

    GnomeXkbInfo *m_xkbInfo;
    QList<KeyboardLayout *> m_keyboardLayouts;
    SubsetModel m_keyboardLayoutsModel;
    AccountsService m_accountsService;
};

#endif // HWKBD_PLUGIN_H
