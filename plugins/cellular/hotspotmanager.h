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
#include <QDBusObjectPath>
/**
 * For exposing dbus data to Qml.
 */

// NM 802.11 Mode
enum class HotspotMode
{
    Unknown,
    Adhoc,
    Infra,
    Ap
};

class HotspotManager : public QObject {
    Q_OBJECT

public:
    explicit HotspotManager(QObject *parent = nullptr);
    ~HotspotManager() {};

    Q_INVOKABLE QByteArray getHotspotName();
    Q_INVOKABLE QString getHotspotPassword();
    Q_INVOKABLE void setupHotspot(QByteArray ssid, QString password, HotspotMode mode = HotspotMode::Ap);
    Q_INVOKABLE bool isHotspotActive();
    Q_INVOKABLE void enableHotspot();
    Q_INVOKABLE void disableHotspot();
    void destroyHotspot();

public Q_SLOTS:
    void newConnection(QDBusObjectPath);

private:
    QByteArray m_ssid;
    QString m_password;
    QString m_settingsPath;
    HotspotMode m_mode;
    QDBusObjectPath m_devicePath;
    bool m_isActive;

    void enableAdhocHotspot();
    void enableApHotspot();

    void setWifiBlock(bool block);
    void setupWpas();
};


#endif
