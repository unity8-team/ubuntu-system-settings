/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cellulardbushelper.h"
#include <QStringList>
#include <QDBusReply>
#include <QtDebug>
#include <QDBusInterface>

namespace {

const QString nm_service("org.freedesktop.NetworkManager");
const QString nm_object("/org/freedesktop/NetworkManager/Settings");
const QString settings_interface("org.freedesktop.NetworkManager.Settings");
const QString connection_interface("org.freedesktop.NetworkManager.Settings.Connection");

}

CellularDbusHelper::CellularDbusHelper(QObject *parent) : QObject(parent) {
}

QString CellularDbusHelper::getHotspotName() {
    return "Ubuntu hotspot";
}

QString CellularDbusHelper::getHotspotPassword() {
    return "qwerty0";
}

void CellularDbusHelper::setHotspotSettings(QString /*ssid*/, QString /*password*/) {
}

bool CellularDbusHelper::isHotspotActive() {
    return false;
}

void CellularDbusHelper::toggleHotspot(bool on) {
    if(on)
        printf("Toggling hotspot on.\n");
    else
        printf("Toggling hotspot off.\n");
}
