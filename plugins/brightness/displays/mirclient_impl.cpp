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

#include "mirclient_impl.h"

#include <QDebug>
#include <QQmlEngine>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>

namespace DisplayPlugin
{
static void mir_display_change_callback(MirConnection *connection,
                                        void *context) {
    MirDisplayConfiguration *conf = mir_connection_create_display_config(
        connection);
    static_cast<MirClientImpl*>(context)->setConfiguration(conf);
}

MirClientImpl::MirClientImpl(QObject *parent)
    : MirClient(parent)
    , m_mir_connection(nullptr)
    , m_configuration(nullptr)
{
    connect();
    if (isConnected()) {
        setConfiguration(
            mir_connection_create_display_config(m_mir_connection)
        );
    }
}

MirClientImpl::~MirClientImpl() {
    mir_display_config_destroy(m_configuration);
    mir_connection_release(m_mir_connection);
}

MirDisplayConfiguration* MirClientImpl::getConfiguration() const {
    return m_configuration;
}

bool MirClientImpl::isConnected() {
    return mir_connection_is_valid(m_mir_connection);
}

void MirClientImpl::setConfiguration(MirDisplayConfiguration *conf) {
    if (m_configuration != conf) {
        m_configuration = conf;
        Q_EMIT configurationChanged();
    }
}

bool MirClientImpl::applyConfiguration(MirDisplayConfiguration *conf) {
    MirWaitHandle* handle = mir_connection_set_base_display_config(
        m_mir_connection, conf
    );

    if (!handle) {
        qWarning() << __PRETTY_FUNCTION__ << "Failed to get handle.";
        return false;
    }

    mir_wait_for(handle);
    QString error(mir_connection_get_error_message(m_mir_connection));
    if (error.isEmpty()) {
        qWarning() << "Mir apply config successfully.";
    } else {
        qWarning() << "Mir configuration error:" << error;
    }
    return error.isEmpty();
}

void MirClientImpl::connect() {
    m_mir_connection = static_cast<MirConnection*>(
        QGuiApplication::platformNativeInterface()
            ->nativeResourceForIntegration("mirConnection")
    );
    if (m_mir_connection == nullptr || !isConnected()) {
        const char *error = "Unknown error";
        if (m_mir_connection != nullptr)
            error = mir_connection_get_error_message(m_mir_connection);
        qWarning() << __PRETTY_FUNCTION__ << "Could not connect to Mir:" << error;
    } else {
        mir_connection_set_display_config_change_callback(
            m_mir_connection, mir_display_change_callback, this);
    }
}
} // DisplayPlugin
