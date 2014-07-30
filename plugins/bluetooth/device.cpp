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

#include <QDBusReply>
#include <QDebug> // qWarning()
#include <QThread>
#include <QTimer>

#include "dbus-shared.h"
#include "device.h"

/***
****
***/

Device::Device(const QMap<QString,QVariant> &properties)
{
    setProperties(properties);
}

Device::Device(const QString &path, QDBusConnection &bus)
{
    initDevice(path, bus);
}

void Device::initDevice(const QString &path, QDBusConnection &bus)
{
    /* whenever any of the properties changes,
       trigger the catch-all deviceChanged() signal */
    QObject::connect(this, SIGNAL(nameChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(iconNameChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(addressChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(pairedChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(trustedChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(typeChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(connectionChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(strengthChanged()), this, SIGNAL(deviceChanged()));

    // init the interfaces that we're supporting.
    initInterface(m_deviceInterface,      path, "org.bluez.Device",      bus);
    initInterface(m_audioInterface,       path, "org.bluez.Audio",       bus);
    initInterface(m_audioSourceInterface, path, "org.bluez.AudioSource", bus);
    initInterface(m_audioSinkInterface,   path, "org.bluez.AudioSink",   bus);
    initInterface(m_headsetInterface,     path, "org.bluez.Headset",     bus);

    Q_EMIT(pathChanged());
}

/***
****
***/

void Device::slotPropertyChanged(const QString      &key,
                                 const QDBusVariant &value)
{
  updateProperty (key, value.variant());
}

void Device::initInterface(QSharedPointer<QDBusInterface> &setme,
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

void Device::setProperties(const QMap<QString,QVariant> &properties)
{
    QMapIterator<QString,QVariant> it(properties);
    while (it.hasNext()) {
        it.next();
        updateProperty(it.key(), it.value());
    }
}

void Device::connectPending()
{
    if (m_paired && !m_trusted) {
        /* Give the device a bit of time to settle.
         * Once service discovery is done, it will call connect() on the
         * pending interfaces.
         */
        QTimer::singleShot(1, this, SLOT(discoverServices()));
    }
}

/***
****
***/

void Device::addConnectAfterPairing(ConnectionMode mode)
{
    m_connectAfterPairing.append(mode);
}

void Device::slotServiceDiscoveryDone(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<void> reply = *call;

    if (!reply.isError()) {
        while (!m_connectAfterPairing.isEmpty()) {
            ConnectionMode mode = m_connectAfterPairing.takeFirst();
            connect(mode);
        }
    } else {
        qWarning() << "Could not initiate service discovery:"
                   << reply.error().message();
    }
    call->deleteLater();
}

void Device::discoverServices()
{
    if (m_deviceInterface) {
        QDBusPendingCall pcall
            = m_deviceInterface->asyncCall("DiscoverServices",
                                           QLatin1String(""));

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall,
                                                                       this);
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                         this,
                         SLOT(slotServiceDiscoveryDone(QDBusPendingCallWatcher*)));
    } else {
        qWarning() << "Can't do service discovery: the device interface is not ready.";
    }
}

void Device::callInterface(const QSharedPointer<QDBusInterface> &interface, const QString &method)
{
    QDBusReply<void> reply;
    constexpr int maxTries = 4;
    int retryCount = 0;

    while (retryCount < maxTries) {
        reply = interface->call(method);
        if (reply.isValid())
            break;
        QThread::msleep(500);
        retryCount++;
    }

    if (retryCount >= maxTries && !reply.isValid()) {
        qWarning() << "Could not" << method << "the interface" << interface->interface()
                   << ":" << reply.error().message();
    }
}

void Device::disconnect(ConnectionMode mode)
{
    QSharedPointer<QDBusInterface> interface;

    if (m_headsetInterface && (mode == HeadsetMode))
        interface = m_headsetInterface;
    else if (m_audioInterface && (mode == Audio))
        interface = m_audioInterface;
    else {
        qWarning() << "Unhandled connection mode" << mode;
        return;
    }

    callInterface(interface, "Disconnect");
}

void Device::connect(ConnectionMode mode)
{
    QSharedPointer<QDBusInterface> interface;

    if (m_headsetInterface && (mode == HeadsetMode))
        interface = m_headsetInterface;
    else if (m_audioInterface && (mode == Audio))
        interface = m_audioInterface;
    else {
        qWarning() << "Unhandled connection mode" << mode;
        return;
    }

    callInterface(interface, "Connect");
}

void Device::slotMakeTrustedDone(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<void> reply = *call;

    if (reply.isError()) {
        qWarning() << "Could not set device as trusted:"
                   << reply.error().message();
    }
    call->deleteLater();
}

void Device::makeTrusted(bool trusted)
{
    QVariant value;
    QDBusVariant variant(trusted);

    value.setValue(variant);

    if (m_deviceInterface) {
        QDBusPendingCall pcall
            = m_deviceInterface->asyncCall("SetProperty", "Trusted", value);

        QDBusPendingCallWatcher *watcher
            = new QDBusPendingCallWatcher(pcall, this);
        QObject::connect(watcher,
                         SIGNAL(finished(QDBusPendingCallWatcher*)),
                         this,
                         SLOT(slotServiceDiscoveryDone(QDBusPendingCallWatcher*)));
    } else {
        qWarning() << "Can't set device trusted before it is added in BlueZ";
    }
}

/***
****
***/

void Device::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT(nameChanged());
    }
}

void Device::setIconName(const QString &iconName)
{
    if (m_iconName != iconName) {
        m_iconName = iconName;
        Q_EMIT(iconNameChanged());
    }
}

void Device::setAddress(const QString &address)
{
    if (m_address != address) {
        m_address = address;
        Q_EMIT(addressChanged());
    }
}

void Device::setType(Type type)
{
    if (m_type != type) {
        m_type = type;
        Q_EMIT(typeChanged());
        updateIcon();
    }
}

void Device::setPaired(bool paired)
{
    if (m_paired != paired) {
        m_paired = paired;
        Q_EMIT(pairedChanged());
    }
}

void Device::setTrusted(bool trusted)
{
    if (m_trusted != trusted) {
        m_trusted = trusted;
        Q_EMIT(trustedChanged());
    }
}

void Device::setConnection(Connection connection)
{
    if (m_connection != connection) {
        m_connection = connection;
        Q_EMIT(connectionChanged());
    }
}

void Device::updateIcon()
{
    /* bluez-provided icon is unreliable? In testing I'm getting
       an "audio-card" icon from bluez for my NoiseHush N700 headset.
       Try to guess the icon from the device type,
       and use the bluez-provided icon as a fallback */

    const auto type = getType();

    switch (type) {
    case Type::Headset:
        setIconName("image://theme/audio-headset-symbolic");
        break;
    case Type::Headphones:
        setIconName("image://theme/audio-headphones-symbolic");
        break;
    case Type::Carkit:
        setIconName("image://theme/audio-carkit-symbolic");
        break;
    case Type::Speakers:
    case Type::OtherAudio:
        setIconName("image://theme/audio-speakers-symbolic");
        break;
    case Type::Mouse:
        setIconName("image://theme/input-mouse-symbolic");
        break;
    case Type::Keyboard:
        setIconName("image://theme/input-keyboard-symbolic");
        break;
    case Type::Cellular:
        setIconName("image://theme/phone-cellular-symbolic");
        break;
    case Type::Smartphone:
        setIconName("image://theme/phone-smartphone-symbolic");
        break;
    case Type::Phone:
        setIconName("image://theme/phone-uncategorized-symbolic");
        break;
    case Type::Computer:
        setIconName("image://theme/computer-symbolic");
        break;
    default: 
        setIconName(QString("image://theme/%1").arg(m_fallbackIconName));
    }
}

void Device::updateConnection()
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

    if (m_isConnected && m_paired && !m_trusted)
        makeTrusted(true);

    setConnection(c);
}

void Device::updateProperty(const QString &key, const QVariant &value)
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
        connectPending();
        updateConnection();
    } else if (key == "Trusted") { // org.bluez.Device
        setTrusted(value.toBool());
    } else if (key == "Icon") { // org.bluez.Device
        m_fallbackIconName = value.toString();
        updateIcon ();
    }
}

/***
****
***/

/* Determine the Type from the bits in the Class of Device (CoD) field.
   https://www.bluetooth.org/en-us/specification/assigned-numbers/baseband */
Device::Type Device::getTypeFromClass (quint32 c)
{
    switch ((c & 0x1f00) >> 8) {
    case 0x01:
        return Type::Computer;

    case 0x02:
        switch ((c & 0xfc) >> 2) {
        case 0x01:
            return Type::Cellular;
        case 0x03:
            return Type::Smartphone;
        case 0x04:
            return Type::Modem;
        default:
            return Type::Phone;
        }
        break;

    case 0x03:
        return Type::Network;

    case 0x04:
        switch ((c & 0xfc) >> 2) {
        case 0x01:
        case 0x02:
            return Type::Headset;

        case 0x05:
            return Type::Speakers;

        case 0x06:
            return Type::Headphones;

        case 0x08:
            return Type::Carkit;

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

