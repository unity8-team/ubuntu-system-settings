/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014-2016 Canonical Ltd.
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
 * Authors:
 *     Antti Kaijanmäki <antti.kaijanmaki@canonical.com>
 */

#ifndef MOCK_NETWORKINGSTATUS_H
#define MOCK_NETWORKINGSTATUS_H

#include <QObject>
#include <QVector>

class Q_DECL_EXPORT MockNetworkingStatus : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MockNetworkingStatus)

    Q_ENUMS(Limitations)
    Q_ENUMS(Status)

    Q_PROPERTY(QVector<Limitations> limitations READ limitations NOTIFY limitationsChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)
    Q_PROPERTY(bool limitedBandwith READ limitedBandwith WRITE setLimitedBandwidth NOTIFY limitedBandwithChanged)

public:
    explicit MockNetworkingStatus(QObject *parent = 0);
    virtual ~MockNetworkingStatus();

    enum Limitations {
        Bandwith
    };

    enum Status {
        Offline,
        Connecting,
        Online
    };

    QVector<Limitations> limitations() const;
    Status status() const;
    bool online() const;
    bool limitedBandwith() const;

    void setLimitedBandwidth(bool limited); // mock only
    Q_INVOKABLE void setStatus(Status status); // mock only

Q_SIGNALS:
    void limitationsChanged();
    void statusChanged(Status value);
    void onlineChanged(bool value);
    void limitedBandwithChanged(bool value);

private:
    QVector<MockNetworkingStatus::Limitations> m_limitations;
    Status m_status;
};

Q_DECLARE_METATYPE(MockNetworkingStatus::Limitations)
Q_DECLARE_METATYPE(QVector<MockNetworkingStatus::Limitations>)
Q_DECLARE_METATYPE(MockNetworkingStatus::Status)

#endif // MOCK_NETWORKINGSTATUS_H
