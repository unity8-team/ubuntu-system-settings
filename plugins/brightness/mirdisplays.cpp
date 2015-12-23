/*
 * Copyright (C) 2015 Canonical Ltd
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
 */

#include <QDebug>
#include <QQmlEngine>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#include "mirdisplays.h"


static void mir_display_change_callback(MirConnection *connection, void *context) {
    qWarning() << "mir_display_change_callback" << context;
    MirDisplayConfiguration *conf = mir_connection_create_display_config(
            connection);
    static_cast<MirDisplays*>(context)->setConfiguration(conf);

}

MirDisplays::MirDisplays(QObject *parent) :
        QObject(parent),
        m_mir_connection(nullptr),
        m_configuration(nullptr) {
    connect();
    if (isConnected()) {
        setConfiguration(
                mir_connection_create_display_config(m_mir_connection));
    }
}


MirDisplays::~MirDisplays() {
    if (m_configuration) {
        mir_display_config_destroy(m_configuration);
    }
}

MirDisplayConfiguration * MirDisplays::getConfiguration() const {
    return m_configuration;
}

bool MirDisplays::isConnected() {
    return mir_connection_is_valid(m_mir_connection);
}

void MirDisplays::setConfiguration(MirDisplayConfiguration * conf) {
    m_configuration = conf;
}

void MirDisplays::applyConfiguration(MirDisplayConfiguration * conf) {
    mir_wait_for(mir_connection_apply_display_config(m_mir_connection, conf));

    const char *error = "No error";
    error = mir_connection_get_error_message(m_mir_connection);
    qWarning() << error;
}

void MirDisplays::connect() {
    qWarning() << "Connecting...";
    m_mir_connection = static_cast<MirConnection*>(
            QGuiApplication::platformNativeInterface()
                ->nativeResourceForIntegration("mirConnection")
    );
    if (m_mir_connection == nullptr || !isConnected()) {
        const char *error = "Unknown error";
        if (m_mir_connection != nullptr)
            error = mir_connection_get_error_message(m_mir_connection);
        qWarning() << error;
    } else {
        qWarning() << "Using mir as display server.";
        mir_connection_set_display_config_change_callback(
                m_mir_connection, mir_display_change_callback, this);
    }
}
