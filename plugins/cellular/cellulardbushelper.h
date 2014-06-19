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

#ifndef CELLULAR_DBUS_HELPER
#define CELLULAR_DBUS_HELPER

#include <QObject>

/**
 * For exposing dbus data to Qml.
 */

class CellularDbusHelper final : public QObject {
    Q_OBJECT

public:
    CellularDbusHelper(QObject *parent = nullptr);
    ~CellularDbusHelper() {};

    Q_INVOKABLE QString getHotspotName();
    Q_INVOKABLE QString getHotspotPassword();
    Q_INVOKABLE void setHotspotSettings(QString ssid, QString password);
    Q_INVOKABLE bool isHotspotActive();
    Q_INVOKABLE void toggleHotspot(bool on);

private:
    QString ssid;
    QString password;

};


#endif
