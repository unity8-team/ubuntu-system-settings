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
 */

#include "hotspot-plugin.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QProcessEnvironment>
#include <QStringList>
#include <QtDBus>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

typedef QMap<QString,QString> VersionDetail;
Q_DECLARE_METATYPE(VersionDetail)

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
    qDBusRegisterMetaType<VersionDetail>();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains(QLatin1String("USS_SHOW_ALL_UI"))) {
        QString showAllS = env.value("USS_SHOW_ALL_UI", QString());

        if(!showAllS.isEmpty()) {
            setVisibility(true);
            return;
        }
    }

    bool supportedDevice(true);

    // FIXME: Checks whether device is mako or flo. See lp:1434591.
    QDBusInterface m_SystemServiceIface("com.canonical.SystemImage",
                                        "/Service",
                                        "com.canonical.SystemImage",
                                        QDBusConnection::systemBus());
    QDBusPendingReply<int, QString, QString, QString, QMap<QString, QString> > reply = m_SystemServiceIface.call("Info");
    reply.waitForFinished();
    if (reply.isValid()) {
        QString device = reply.argumentAt<1>();
        supportedDevice = !(device == "mako" || device == "flo");
    }

    setVisibility(supportedDevice);
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
