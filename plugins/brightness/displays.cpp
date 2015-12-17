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
#include "displays.h"

typedef struct MirState {
    MirConnection *connection;
} MirState;

static void mir_connection_callback(MirConnection *new_connection, void *context) {
    qWarning() << "Connection callback.";
    ((MirState*)context)->connection = new_connection;
}

static void mir_display_change_callback(MirConnection *connection, void *context) {
    qWarning() << "mir_display_change_callback";
    // ((Displays*)context)->updateAvailableDisplays();
}

Displays::Displays(QObject *parent) :
    QObject(parent),
    m_displaysModel(this) {
    if(makeDisplayServerConnection()) {
        qWarning() << "displays connected";
        updateAvailableDisplays();
        mir_connection_set_display_config_change_callback(
                m_mir_connection, mir_display_change_callback, this);
    } else {
        qWarning() << "displays not connected";
    }
}

Displays::~Displays() {
    mir_display_config_destroy(m_configuration);
}

bool Displays::makeDisplayServerConnection() {

    qWarning() << "Connecting...";
    m_mir_connection = static_cast<MirConnection*>(QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("mirConnection"));
    qWarning() << "Connected!";
    if (m_mir_connection == nullptr || !mir_connection_is_valid(m_mir_connection)) {
        const char *error = "Unknown error";
        if (m_mir_connection != nullptr)
            error = mir_connection_get_error_message(m_mir_connection);
        qWarning() << error;
        return false;
    } else {
        qWarning() << "Using mir as display server.";
        return true;
    }
    return false;
}

void Displays::updateAvailableDisplays() {
    MirDisplayConfiguration *conf = mir_connection_create_display_config(
            m_mir_connection);

    for (unsigned int i = 0; i < conf->num_outputs; ++i) {
        MirDisplayOutput output = conf->outputs[i];
        qWarning() << "output" << i;
        QSharedPointer<Display> display(new Display(&output));
        m_displaysModel.addDisplay(display);
    }
    m_configuration = conf;
}

QAbstractItemModel * Displays::displays() {
    auto ret = &m_displaysModel;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

void Displays::applyDisplayConfiguration() {
    qWarning() << "apply config";
    MirDisplayOutput * outs[m_displaysModel.rowCount()] = {};
    for (int i = 0; i < m_displaysModel.rowCount(); i++) {
        int x = (int)i;
        QModelIndex ix = m_displaysModel.index(x, 0, QModelIndex());
        QString name = m_displaysModel.data(ix).toString();
        QSharedPointer<Display> display = m_displaysModel.getDisplay(name);
        outs[i] = display->output();
    }

    for (unsigned  int i = 0; i < m_configuration->num_outputs; ++i) {
        MirDisplayOutput output = m_configuration->outputs[i];
        qWarning() << "output" << i << "used" << output.used << "mode" << output.current_mode;

    }
    qWarning() << "sat new outputs";
    m_configuration->outputs = reinterpret_cast<MirDisplayOutput*>(outs);

    for (unsigned int i = 0; i < m_configuration->num_outputs; ++i) {
        MirDisplayOutput output = m_configuration->outputs[i];
        qWarning() << "output" << i << "used" << output.used << "mode" << output.current_mode;
    }

    mir_wait_for (mir_connection_apply_display_config(m_mir_connection, m_configuration));

    const char *error = "No error";
    error = mir_connection_get_error_message(m_mir_connection);
    qWarning() << error;
}
