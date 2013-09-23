/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Charles Kerr <charles.kerr@canonical.com>
 */

#include <QDebug>
#include <QDBusReply>

#include "dbus-shared.h"
#include "device.h"

/***
****
***/

Device::Device(const QString &path, QDBusConnection &bus)
{
    /* whenever any of the properties changes,
       trigger the catch-all deviceChanged() signal */
    QObject::connect(this, SIGNAL(nameChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(iconNameChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(addressChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(pairedChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(typeChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(connectionChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(strengthChanged()), this, SIGNAL(deviceChanged()));

    // init the interfaces that we're supporting.
    initInterface(m_deviceInterface,      path, "org.bluez.Device",      bus);
    initInterface(m_audioInterface,       path, "org.bluez.Audio",       bus);
    initInterface(m_audioSourceInterface, path, "org.bluez.AudioSource", bus);
    initInterface(m_headsetInterface,     path, "org.bluez.Headset",     bus);
}

/***
****
***/

void
Device::slotPropertyChanged(const QString      &key,
                            const QDBusVariant &value)
{
  updateProperty (key, value.variant());
}

void
Device::initInterface(QSharedPointer<QDBusInterface> &setme,
                      const QString                  &path,
                      const QString                  &interfaceName,
                      QDBusConnection                &bus)
{
    const QString service = "org.bluez";

    auto i = new QDBusInterface(service, path, interfaceName, bus);

    if (!i->isValid()) {
        delete i;
        i = 0;
    } else {
        if (!bus.connect(service, path, interfaceName, "PropertyChanged",
                         this, SLOT(slotPropertyChanged(const QString&, const QDBusVariant&))))
            qWarning() << "Unable to connect to " << interfaceName << "::PropertyChanged on" << path;
    }

    setme.reset(i);

    if (setme && setme->isValid()) {
        QDBusReply<QMap<QString,QVariant> > properties = setme->call("GetProperties");
        if (properties.isValid())
            setProperties(properties.value());
    }
}

void
Device::setProperties(const QMap<QString,QVariant> &properties)
{
    QMapIterator<QString,QVariant> it(properties);
    while (it.hasNext()) {
        it.next();
        updateProperty(it.key(), it.value());
    }
}

/***
****
***/

void
Device::disconnect(ConnectionMode mode)
{
    if (m_headsetInterface && (mode == HeadsetMode))
        m_headsetInterface->asyncCall("Disconnect");
    else if (m_audioInterface && (mode == Audio))
        m_audioInterface->asyncCall("Disconnect");
    else if (m_audioSourceInterface && (mode == AudioSource))
        m_audioSourceInterface->asyncCall("Disconnect");
    else
        qWarning() << "Unhandled connection mode" << mode;
}

void
Device::connect(ConnectionMode mode)
{
    if (m_headsetInterface && (mode == HeadsetMode))
        m_headsetInterface->asyncCall("Connect");
    else if (m_audioInterface && (mode == Audio))
        m_audioInterface->asyncCall("Connect");
    else if (m_audioSourceInterface && (mode == AudioSource))
        m_audioSourceInterface->asyncCall("Connect");
    else
        qWarning() << "Unhandled connection mode" << mode;
}

/***
****
***/

void
Device::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT(nameChanged());
    }
}

void
Device::setIconName(const QString &iconName)
{
    if (m_iconName != iconName) {
        m_iconName = iconName;
        Q_EMIT(iconNameChanged());
    }
}

void
Device::setAddress(const QString &address)
{
    if (m_address != address) {
        m_address = address;
        Q_EMIT(addressChanged());
    }
}

void
Device::setType(Type type)
{
    if (m_type != type) {
        m_type = type;
        Q_EMIT(typeChanged());
        updateIcon();
    }
}

void
Device::setPaired(bool paired)
{
    if (m_paired != paired) {
        m_paired = paired;
        Q_EMIT(pairedChanged());
    }
}

void
Device::setConnection(Connection connection)
{
    if (m_connection != connection) {
        m_connection = connection;
        Q_EMIT(connectionChanged());
    }
}

void
Device::updateIcon()
{
    /* bluez-provided icon is unreliable? In testing I'm getting
       an "audio-card" icon from bluez for my NoiseHush N700 headset.
       Try to guess the icon from the device type,
       and use the bluez-provided icon as a fallback */

    const auto type = getType();

    if (type == Type::Headset)
        setIconName("image://theme/audio-headset");
    else if (type == Type::Phone)
        setIconName("image://theme/phone");
    else if (!m_fallbackIconName.isEmpty())
        setIconName(QString("image://theme/%1").arg(m_fallbackIconName));
}

void
Device::updateConnection()
{
    Connection c;
  
    /* The "State" property is a little more useful this "Connected" bool
       because the former tells us Bluez *knows* a device is connecting.
       So use "Connected" only as a fallback */

    if ((m_state == "connected") || (m_state == "playing"))
        c = Connection::Connected;
    else if (m_state == "connecting")
        c = Connection::Connecting;
    else if (m_state == "disconnected")
        c = Connection::Disconnected;
    else
        c = m_isConnected ? Connection::Connected : Connection::Disconnected;

    setConnection(c);
}

void
Device::updateProperty(const QString &key, const QVariant &value)
{
    if (key == "Name") { // org.bluez.Device
        setName(value.toString());
    } else if (key == "Address") { // org.bluez.Device
        setAddress(value.toString());
    } else if (key == "State") { // org.bluez.Audio, org.bluez.Headset
        m_state = value.toString();
        updateConnection();
    } else if (key == "Connected") {
        m_isConnected = value.toBool();
        updateConnection();
    } else if (key == "Class") { // org.bluez.Device
        setType(getTypeFromClass(value.toUInt()));
    } else if (key == "Paired") { // org.bluez.Device
        setPaired(value.toBool());
    } else if (key == "Icon") { // org.bluez.Device
        m_fallbackIconName = value.toString();
        updateIcon ();
    }
}

/***
****
***/

Device::Type
Device::getTypeFromClass (quint32 c)
{
    switch ((c & 0x1f00) >> 8) {
    case 0x01:
        return Type::Computer;

    case 0x02:
        switch ((c & 0xfc) >> 2) {
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x05:
            return Type::Phone;

        case 0x04:
            return Type::Modem;
        }
        break;

    case 0x03:
        return Type::Network;

    case 0x04:
        switch ((c & 0xfc) >> 2) {
        case 0x01:
        case 0x02:
            return Type::Headset;

        case 0x06:
            return Type::Headphones;

        case 0x0b: // vcr
        case 0x0c: // video camera
        case 0x0d: // camcorder
            return Type::Video;

        default:
            return Type::OtherAudio;
        }

    case 0x05:
        switch ((c & 0xc0) >> 6) {
        case 0x00:
            switch ((c & 0x1e) >> 2) {
            case 0x01:
            case 0x02:
                return Type::Joypad;
            }
            break;

        case 0x01:
            return Type::Keyboard;

        case 0x02:
            switch ((c & 0x1e) >> 2) {
            case 0x05:
                return Type::Tablet;
            default:
                return Type::Mouse;
            }
        }
        break;

    case 0x06:
        if ((c & 0x80) != 0)
            return Type::Printer;
        if ((c & 0x20) != 0)
            return Type::Camera;
        break;
    }

    return Type::Other;
}

