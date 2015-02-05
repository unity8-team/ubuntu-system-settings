/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp agent.xml -a agentadaptor
 *
 * qdbusxml2cpp is Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef AGENTADAPTOR_H_1423046874
#define AGENTADAPTOR_H_1423046874

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface org.bluez.Agent
 */
class AgentAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.bluez.Agent")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.bluez.Agent\">\n"
"    <method name=\"RequestPinCode\">\n"
"      <annotation value=\"\" name=\"org.freedesktop.DBus.GLib.Async\"/>\n"
"      <arg type=\"o\" name=\"device\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"pincode\"/>\n"
"    </method>\n"
"    <method name=\"RequestPasskey\">\n"
"      <annotation value=\"\" name=\"org.freedesktop.DBus.GLib.Async\"/>\n"
"      <arg type=\"o\" name=\"device\"/>\n"
"      <arg direction=\"out\" type=\"u\" name=\"passkey\"/>\n"
"    </method>\n"
"    <method name=\"DisplayPasskey\">\n"
"      <annotation value=\"\" name=\"org.freedesktop.DBus.GLib.Async\"/>\n"
"      <arg type=\"o\" name=\"device\"/>\n"
"      <arg type=\"u\" name=\"passkey\"/>\n"
"      <arg type=\"q\" name=\"entered\"/>\n"
"    </method>\n"
"    <method name=\"RequestConfirmation\">\n"
"      <annotation value=\"\" name=\"org.freedesktop.DBus.GLib.Async\"/>\n"
"      <arg type=\"o\" name=\"device\"/>\n"
"      <arg type=\"u\" name=\"passkey\"/>\n"
"    </method>\n"
"    <method name=\"Cancel\">\n"
"      <annotation value=\"\" name=\"org.freedesktop.DBus.GLib.Async\"/>\n"
"    </method>\n"
"    <method name=\"Release\">\n"
"      <annotation value=\"\" name=\"org.freedesktop.DBus.GLib.Async\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    AgentAdaptor(QObject *parent);
    virtual ~AgentAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void Cancel();
    void DisplayPasskey(const QDBusObjectPath &device, uint passkey, ushort entered);
    void Release();
    void RequestConfirmation(const QDBusObjectPath &device, uint passkey);
    uint RequestPasskey(const QDBusObjectPath &device);
    QString RequestPinCode(const QDBusObjectPath &device);
Q_SIGNALS: // SIGNALS
};

#endif
