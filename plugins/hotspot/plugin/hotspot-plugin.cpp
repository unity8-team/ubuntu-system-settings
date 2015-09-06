/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
 *
 * FIXME: This plugin exists because hotspot does not work on mako. See lp:1434591.
 *
 */

#include "hotspot-plugin.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QProcessEnvironment>
#include <QStringList>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

class HotspotItem: public ItemBase
{
    Q_OBJECT

public:
    explicit HotspotItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);

};


HotspotItem::HotspotItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QDBusInterface m_SystemServiceIface ("com.canonical.SystemImage",
                                         "/Service",
                                         "com.canonical.SystemImage",
                                         QDBusConnection::systemBus());

    bool showAll(false);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains(QLatin1String("USS_SHOW_ALL_UI"))) {
        QString showAllS = env.value("USS_SHOW_ALL_UI", QString());
        showAll = !showAllS.isEmpty();
    }

    bool isNotMako(false);
    QDBusPendingReply<int, QString, QString, QString, QMap<QString, QString> > reply = m_SystemServiceIface.call("Info");
    reply.waitForFinished();
    if (reply.isValid()) {
        isNotMako = reply.argumentAt<1>() != "mako";
    }

    setVisibility(isNotMako || showAll);
}

void HotspotItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *HotspotPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new HotspotItem(staticData, parent);
}

#include "hotspot-plugin.moc"
