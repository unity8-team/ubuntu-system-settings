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
#include "../mirdisplays.h"

#include <QDebug>
#include <QDBusInterface>
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

private:
    int getNumberOfDisplays();
};


BrightnessItem::BrightnessItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QDBusInterface m_powerdIface ("com.canonical.powerd",
                                  "/com/canonical/powerd",
                                  "com.canonical.powerd",
                                  QDBusConnection::systemBus());

    // Hide the plugin if powerd isn't running; it's redundant currentlys
    //setVisibility(m_powerdIface.isValid());
    setVisibility(true);

    if (getNumberOfDisplays() == 0) {
        setDisplayName(_("Brightness"));
    } else {
        setDisplayName(_("Brightness & Display"));
    }
}

int BrightnessItem::getNumberOfDisplays() {
    MirDisplays mirDisplays;
    int outputs = 0;

    if (mirDisplays.isConnected()) {
        MirDisplayConfiguration *conf = mirDisplays.getConfiguration();
        outputs = conf->num_outputs;
    }
    return outputs;
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
