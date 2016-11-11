/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cgmanager.h"

// QPA mirserver
#include <logging.h>

#include <cgmanager/cgmanager.h>

#include <QDBusArgument>
#include <QDBusMessage>

using namespace qtmir;

// According to D-Bus interface defined in:
// https://github.com/lxc/cgmanager/blob/master/org.linuxcontainers.cgmanager.xml

CGManager::CGManager(QObject *parent)
    : QObject(parent)
{
}

CGManager::~CGManager()
{
    QDBusConnection::disconnectFromPeer("cgmanager");
}

QDBusConnection CGManager::getConnection()
{
    auto connection = QDBusConnection("cgmanager");

    if (!connection.isConnected()) {
        connection = QDBusConnection::connectToPeer(CGMANAGER_DBUS_PATH, "cgmanager");
        if (!connection.isConnected()) {
            qCWarning(QTMIR_DBUS, "CGManager: Failed to connect to %s", CGMANAGER_DBUS_PATH);
        }
    }

    return connection;
}

QString CGManager::getCGroupOfPid(const QString &controller, pid_t pid)
{
    auto connection = getConnection();
    if (!connection.isConnected()) {
        return QString();
    }

    auto message = QDBusMessage::createMethodCall(QString() /*service*/, m_path, m_interface, "GetPidCgroup");

    QList<QVariant> arguments;
    arguments << QVariant(controller);
    arguments << QVariant((int)pid);

    message.setArguments(arguments);

    QDBusMessage reply = connection.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage) {
        return reply.arguments().at(0).toString();
    } else {
        Q_ASSERT(reply.type() == QDBusMessage::ErrorMessage);
        qCWarning(QTMIR_DBUS) << "CGManager::getCGroupOfPid failed." << reply.errorMessage();
        return QString();
    }
}

QSet<pid_t> CGManager::getTasks(const QString &controller, const QString &cgroup)
{
    auto connection = getConnection();
    if (!connection.isConnected()) {
        return QSet<pid_t>();
    }

    auto message = QDBusMessage::createMethodCall(QString() /*service*/, m_path, m_interface, "GetTasks");

    QList<QVariant> arguments;
    arguments << QVariant(controller);
    arguments << QVariant(cgroup);

    message.setArguments(arguments);

    QDBusMessage reply = connection.call(message);

    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCWarning(QTMIR_DBUS) << "CGManager::getCGroupOfPid failed." << reply.errorMessage();
        return QSet<pid_t>();
    }

    Q_ASSERT(reply.type() == QDBusMessage::ReplyMessage);

    auto arg = reply.arguments().at(0).value<QDBusArgument>();
    Q_ASSERT(arg.currentType() == QDBusArgument::ArrayType);

    auto pidList = qdbus_cast<QList<int>>(arg);
    Q_ASSERT(pidList.count() > 0);

    QSet<pid_t> pidSet;
    for (int i = 0; i < pidList.count(); ++i) {
        pidSet << (pid_t)pidList[i];
    }
    return pidSet;
}
