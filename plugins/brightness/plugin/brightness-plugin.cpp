/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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

#include "brightness-plugin.h"
#include <hybris/properties/properties.h>

#include <QDebug>
#include <QDBusInterface>
#include <QProcessEnvironment>
#include <QStringList>
#include <SystemSettings/ItemBase>

#include <libintl.h>
QString _(const char *text){
    return QString::fromUtf8(dgettext(0, text));
}

using namespace SystemSettings;

class BrightnessItem: public ItemBase
{
    Q_OBJECT

public:
    explicit BrightnessItem(const QVariantMap &staticData, QObject *parent = 0);
    void setDisplayName(const QString &name);
    void setVisibility(bool visible);

};


BrightnessItem::BrightnessItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    setDisplayName(_("Brightness"));

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains(QLatin1String("USS_SHOW_ALL_UI"))) {
        QString showAllS = env.value("USS_SHOW_ALL_UI", "");

        if(!showAllS.isEmpty()) {
            setVisibility(true);
            return;
        }
    }

    QDBusInterface m_powerdIface ("com.canonical.powerd",
                                  "/com/canonical/powerd",
                                  "com.canonical.powerd",
                                  QDBusConnection::systemBus());

    // Hide the plugin if powerd isn't running; it's currently redundant
    setVisibility(m_powerdIface.isValid());

    char widi[PROP_VALUE_MAX] = "";
    property_get("ubuntu.widi.supported", widi, "0");
    // We want to log this property to help aid debugging
    qWarning() << Q_FUNC_INFO << "ubuntu.widi.supported:" << widi;

    if (strcmp(widi, "0") != 0) {
        setDisplayName(_("Brightness & Display"));
    }
}

void BrightnessItem::setDisplayName(const QString &name)
{
    setName(name);
}

void BrightnessItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *BrightnessPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new BrightnessItem(staticData, parent);
}

#include "brightness-plugin.moc"
