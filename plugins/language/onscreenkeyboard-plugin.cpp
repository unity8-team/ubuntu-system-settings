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

#include <QStandardPaths>
#include <QtDebug>
#include "onscreenkeyboard-plugin.h"

#define UBUNTU_KEYBOARD_SCHEMA_ID "com.canonical.keyboard.maliit"

#define KEY_ENABLED_LAYOUTS "enabled-languages"
#define KEY_CURRENT_LAYOUT  "active-language"
#define KEY_PLUGIN_PATHS "plugin-paths"

#define LAYOUTS_DIR "maliit/plugins/com/ubuntu/lib"

OnScreenKeyboardPlugin::OnScreenKeyboardPlugin(QObject *parent) :
    QObject(parent),
    m_maliitSettings(g_settings_new(UBUNTU_KEYBOARD_SCHEMA_ID))
{
    GVariantIter *iter;
    const gchar *path;

    QString layoutPath = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation, LAYOUTS_DIR,
        QStandardPaths::LocateDirectory
    );
    if (!layoutPath.isEmpty()) {
        m_layoutPaths.append(layoutPath);
    }

    g_settings_get(m_maliitSettings, KEY_PLUGIN_PATHS, "as", &iter);
    for (int i(0); g_variant_iter_next(iter, "&s", &path); i++) {
        m_layoutPaths.append(path);
    }
    updateEnabledLayouts();
    updateKeyboardLayouts();
    updateKeyboardLayoutsModel();
}


OnScreenKeyboardPlugin::~OnScreenKeyboardPlugin()
{
    if (m_maliitSettings != nullptr) {
        g_signal_handlers_disconnect_by_data(m_maliitSettings, this);
        g_object_unref(m_maliitSettings);
    }

    for (QList<KeyboardLayout *>::const_iterator
         i(m_keyboardLayouts.begin()); i != m_keyboardLayouts.end(); ++i)
        delete *i;
}

SubsetModel *
OnScreenKeyboardPlugin::keyboardLayoutsModel()
{
    return &m_keyboardLayoutsModel;
}

void
OnScreenKeyboardPlugin::keyboardLayoutsModelChanged()
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

    return QString::localeAwareCompare(name0, name1) < 0;
}

void
OnScreenKeyboardPlugin::updateEnabledLayouts()
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
OnScreenKeyboardPlugin::updateKeyboardLayouts()
{
    m_keyboardLayouts.clear();

    for (int i = 0; i < m_layoutPaths.count(); i++) {
        QDir layoutsDir(m_layoutPaths.at(i));
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
    }

    qSort(m_keyboardLayouts.begin(), m_keyboardLayouts.end(), compareLayouts);
}

void enabledLayoutsChanged(GSettings *settings,
                           gchar     *key,
                           gpointer   user_data);

void
OnScreenKeyboardPlugin::updateKeyboardLayoutsModel()
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
OnScreenKeyboardPlugin::enabledLayoutsChanged()
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

    OnScreenKeyboardPlugin *plugin(static_cast<OnScreenKeyboardPlugin *>(user_data));
    plugin->enabledLayoutsChanged();
}

void OnScreenKeyboardPlugin::setCurrentLayout(const QString &code)
{
    for (int i = 0; i < m_layoutPaths.count(); i++) {
        QFileInfo fileInfo(QDir(m_layoutPaths.at(i)), code);

        if (fileInfo.exists() && fileInfo.isDir()) {
            g_settings_set_string(m_maliitSettings,
                                  KEY_CURRENT_LAYOUT, code.toStdString().c_str());

            updateEnabledLayouts();
        }
    }
}
