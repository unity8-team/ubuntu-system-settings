/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#include "fakenetworkmanager.h"

FakeNetworkManager::FakeNetworkManager(const QVariantMap &parameters,
                                       QObject *parent)
    : QObject(parent)
    , m_dbusTestRunner()
    , m_dbusMock(m_dbusTestRunner)
{
    DBusMock::registerMetaTypes();
    qDBusRegisterMetaType<QMap<QString, QString> >();

    /* NETWORK_MANAGER_TEMPLATE is the path to the networkmanager.py file,
    defined in the consuming tests makefile. */
    m_dbusMock.registerTemplate(NM_SERVICE, NETWORK_MANAGER_TEMPLATE, parameters,
                                QDBusConnection::SystemBus);

    m_dbusTestRunner.startServices();
    m_nmMock = new QDBusInterface(NM_SERVICE,
                                  NM_MAIN_OBJECT,
                                  NM_IFACE,
                                  m_dbusTestRunner.systemConnection());
}

FakeNetworkManager::~FakeNetworkManager()
{
    delete m_nmMock;
}

QVariant FakeNetworkManager::getProperty(const QString &path,
                                         const QString &interface,
                                         const QString &property)
{
    QDBusInterface iface(NM_SERVICE, path,
                         FREEDESKTOP_PROPERTIES_IFACE,
                         m_dbusTestRunner.systemConnection());

    QDBusReply<QVariant> reply = iface.call("Get", interface, property);

    if (reply.isValid()) {
        return reply.value();
    } else {
        qWarning() << "Error getting property from mock:"
                   << reply.error().message();
    }

    return reply.isValid() ? reply.value() : QVariant();
}
