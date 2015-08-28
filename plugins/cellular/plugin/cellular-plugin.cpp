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

#include "cellular-plugin.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>
#include <QStringList>
#include <SystemSettings/ItemBase>

using namespace SystemSettings;

class CellularItem: public ItemBase
{
    Q_OBJECT

public:
    explicit CellularItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);

};


CellularItem::CellularItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QDBusInterface m_ofonoIface ("org.ofono.Manager",
                                  "/",
                                  "org.ofono.Manager",
                                  QDBusConnection::systemBus());

    QDBusReply<QStringList> reply = m_ofonoIface.call("GetModems");
    if (reply.isValid()) {
        if (reply.value().length() > 0) {
            setVisibility(true);
            return;
        }
    }
    // Hide the plugin if there are no modems present
    setVisibility(false);
}

void CellularItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *CellularPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new CellularItem(staticData, parent);
}

#include "cellular-plugin.moc"
