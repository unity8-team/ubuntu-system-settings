/*
 * Copyright (C) 2014, 2015 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *    Jonas G. Drange <jonas.drange@canonical.com>
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
    Ap,
    P2p
};

class HotspotManager : public QObject {
    Q_OBJECT
    Q_PROPERTY( bool active
                READ active
                WRITE setActive
                NOTIFY activeChanged)
    Q_INVOKABLE QByteArray getHotspotName();
    Q_INVOKABLE QString getHotspotPassword();
    Q_INVOKABLE void setupHotspot(QByteArray ssid, QString password, HotspotMode mode = HotspotMode::Ap);

public:
    explicit HotspotManager(QObject *parent = nullptr);
    ~HotspotManager() {};

    bool active() const;
    void setActive(bool);
    void destroyHotspot();

Q_SIGNALS:
    void activeChanged(bool active);
    void establishing();
    void hotspotFailed(const QString &message);

public Q_SLOTS:
    void newConnection(QDBusObjectPath);
    void devicedAdded(QDBusObjectPath);

private:
    bool m_active;
    QByteArray m_ssid;
    QString m_password;
    QString m_settingsPath;
    HotspotMode m_mode;
    QDBusObjectPath m_devicePath;

    bool enableHotspot();
    bool disableHotspot();
};


#endif
