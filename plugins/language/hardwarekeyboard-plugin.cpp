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

#include <QDBusMetaType>
#include <QtAlgorithms>
#include <QtDebug>

#include <gio/gio.h>

#include "hardwarekeyboard-plugin.h"

#define INPUT_SOURCE_TYPE_XKB "xkb"
#define SOURCES_CONFIG_SCHEMA_ID "org.gnome.desktop.input-sources"
#define SOURCES_KEY "sources"

typedef QList<QMap<QString, QString>> StringMapList;
Q_DECLARE_METATYPE(StringMapList)

HardwareKeyboardPlugin::HardwareKeyboardPlugin(QObject *parent) :
    QObject(parent),
    m_sourcesSettings(g_settings_new(SOURCES_CONFIG_SCHEMA_ID))
{
    qDBusRegisterMetaType<StringMapList>();
    m_xkbInfo = gnome_xkb_info_new();

    updateKeyboardLayouts();
    updateKeyboardLayoutsModel();
}


HardwareKeyboardPlugin::~HardwareKeyboardPlugin()
{
    if (m_xkbInfo != nullptr) {
        g_object_unref(m_xkbInfo);
    }

    qDeleteAll(m_keyboardLayouts);

    g_object_unref(m_sourcesSettings);
}

SubsetModel *
HardwareKeyboardPlugin::keyboardLayoutsModel()
{
    return &m_keyboardLayoutsModel;
}

void
HardwareKeyboardPlugin::keyboardLayoutsModelChanged()
{
    QVariant answer = m_accountsService.getUserProperty(
                "org.freedesktop.Accounts.User",
                "InputSources");
    StringMapList maps;
    if (answer.isValid()) {
        QDBusArgument arg = answer.value<QDBusArgument>();
        maps = qdbus_cast<StringMapList>(arg);
    } else {
        qCritical() << "failed to get input sources";
        return;
    }

    StringMapList finalMaps;
    for (int i = 0; i < maps.size(); i++) {
        QMap<QString, QString> m = maps.at(i);

        // Keep any maps not of xkb type (ibus e.g.)
        if (!m.contains(INPUT_SOURCE_TYPE_XKB)) {
            finalMaps.append(m);
        }
    }

    // Update maps with what the user selected.
    QList<int> subset = m_keyboardLayoutsModel.subset();

    // The first top item selected by the user will appear as the first map.
    QListIterator<int> it(subset);
    it.toBack();
    while (it.hasPrevious()) {
        QMap<QString, QString> m = QMap<QString, QString>();
        KeyboardLayout* layout = m_keyboardLayouts.at(it.previous());
        m.insert(INPUT_SOURCE_TYPE_XKB, layout->name());
        finalMaps.prepend(m);
    }

    m_accountsService.customSetUserProperty(
            "SetInputSources", QVariant::fromValue(finalMaps));

    // Save the config settings (for the keyboard indicator)
    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("a(ss)"));
    Q_FOREACH(const auto & keymapPair, finalMaps) {
        g_variant_builder_add(&builder, "(ss)", keymapPair.firstKey().toUtf8().constData(),
                              keymapPair.first().toUtf8().constData());
    }
    g_settings_set_value(m_sourcesSettings, SOURCES_KEY, g_variant_builder_end(&builder));
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
HardwareKeyboardPlugin::updateKeyboardLayouts()
{
    GList *sources, *tmp;
    gchar *source_id = NULL;
    const gchar *display_name;
    const gchar *short_name;
    const gchar *xkb_layout;
    const gchar *xkb_variant;
    sources = gnome_xkb_info_get_all_layouts(m_xkbInfo);

    m_keyboardLayouts.clear();

    for (tmp = sources; tmp != NULL; tmp = tmp->next) {
        g_free (source_id);
        source_id = g_strconcat(INPUT_SOURCE_TYPE_XKB, tmp->data, NULL);

        gnome_xkb_info_get_layout_info(m_xkbInfo, (const gchar *)tmp->data,
        &display_name, &short_name, &xkb_layout, &xkb_variant);

        KeyboardLayout *layout(new KeyboardLayout((const gchar *)tmp->data,
                                                  short_name,
                                                  display_name,
                                                  xkb_variant));
        if (!layout->language().isEmpty())
            m_keyboardLayouts += layout;
        else
            delete layout;

    }
    g_free(source_id);
    g_list_free(sources);
    qSort(m_keyboardLayouts.begin(), m_keyboardLayouts.end(), compareLayouts);
}

void
HardwareKeyboardPlugin::updateKeyboardLayoutsModel()
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

    connect(&m_keyboardLayoutsModel,
            SIGNAL(subsetChanged()),
            SLOT(keyboardLayoutsModelChanged()));
}

void
HardwareKeyboardPlugin::enabledLayoutsChanged()
{
    QList<int> subset;

    QVariant answer = m_accountsService.getUserProperty(
                "org.freedesktop.Accounts.User",
                "InputSources");

    if (answer.isValid()) {
        QDBusArgument arg = answer.value<QDBusArgument>();
        StringMapList list = qdbus_cast<StringMapList>(arg);

        for (int i = 0; i < list.length(); ++i) {
            for (int j = 0; j < m_keyboardLayouts.length(); j++) {
                if (m_keyboardLayouts[j]->name() == list.at(i)[INPUT_SOURCE_TYPE_XKB]) {
                    subset += j;
                    break;
                }
            }
        }
        m_keyboardLayoutsModel.setSubset(subset);
    } else {
        qCritical() << "failed to get input sources";
    }
}

void HardwareKeyboardPlugin::setCurrentLayout(const QString &code)
{
    Q_UNUSED(code);
    // TODO: Implement.
}

void HardwareKeyboardPlugin::requestCurrentLayoutMove(const int from, const int to) {
    m_keyboardLayoutsModel.moveSubsetRow(from, to);
    keyboardLayoutsModelChanged();
}
