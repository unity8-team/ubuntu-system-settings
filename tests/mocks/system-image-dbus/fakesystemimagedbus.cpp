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

#include "fakesystemimagedbus.h"

FakeSystemImageDbus::FakeSystemImageDbus(const QVariantMap &parameters,
                                 QObject *parent)
    : QObject(parent)
    , m_dbusTestRunner()
    , m_dbusMock(m_dbusTestRunner)
{
    DBusMock::registerMetaTypes();
    qDBusRegisterMetaType<QMap<QString, QString> >();

    /* SYSTEM_IMAGE_TEMPLATE is the path to the systemimage.py file, defined in
    the consuming test's makefile. */
    m_dbusMock.registerTemplate(SI_SERVICE, SYSTEM_IMAGE_DBUS_TEMPLATE, parameters,
                                QDBusConnection::SystemBus);

    m_dbusTestRunner.startServices();
    m_siMock = new QDBusInterface(SI_SERVICE,
                                  SI_MAIN_OBJECT,
                                  SI_IFACE,
                                  m_dbusTestRunner.systemConnection());
}

FakeSystemImageDbus::~FakeSystemImageDbus()
{
    delete m_siMock;
}
