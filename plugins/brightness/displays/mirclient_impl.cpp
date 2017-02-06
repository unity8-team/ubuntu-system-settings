/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
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
#include "output/mir_output.h"
#include "../../../src/i18n.h"

#include <mir_toolkit/client_types.h>
#include <mir_toolkit/mir_error.h>

#include <QDebug>
#include <QQmlEngine>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>

namespace DisplayPlugin
{
static void mir_display_change_callback(MirConnection *connection,
                                        void *context)
{
    auto mirClient = static_cast<MirClientImpl*>(context);
    auto conf = mir_connection_create_display_configuration(
        connection);
    mirClient->setConfiguration(conf);
    qWarning() << "Mir apply config successfully.";
}

static void mir_error_callback(MirConnection *connection,
                               MirError const* error,
                               void *context)
{
    Q_UNUSED(connection);
    auto mirClient = static_cast<MirClientImpl*>(context);
    auto domain = mir_error_get_domain(error);
    auto errorCode = mir_error_get_code(error);

    QString msg;
    switch (domain) {
    case MirErrorDomain::mir_error_domain_display_configuration:
        switch ((MirDisplayConfigurationError) errorCode) {
        case MirDisplayConfigurationError::mir_display_configuration_error_unauthorized:
            msg = SystemSettings::_(
                "Client is not permitted to change global display configuration"
            );
            break;
        case MirDisplayConfigurationError::mir_display_configuration_error_in_progress:
            msg = SystemSettings::_(
                "A global configuration change request is already pending"
            );
            break;
        case MirDisplayConfigurationError::mir_display_configuration_error_no_preview_in_progress:
            msg = SystemSettings::_(
                "A cancel request was received, but no global display configuration preview is in progress"
            );
            break;
        case MirDisplayConfigurationError::mir_display_configuration_error_rejected_by_hardware:
            msg = SystemSettings::_(
                "Display configuration was attempted but was rejected by the hardware"
            );
            break;
        }
        break;
    case MirErrorDomain::mir_error_domain_input_configuration:
        msg = SystemSettings::_(
            "Input configuration was attempted but was rejected by driver"
        );
        break;
    }

    // TRANSLATORS: %1 is the reason why configuration failed.
    mirClient->onConfigurationFailed(
        SystemSettings::_("Configuration failed: %1.").arg(msg)
    );

    mirClient->setConfiguration(Q_NULLPTR);
}

MirClientImpl::MirClientImpl(QObject *parent)
    : MirClient(parent)
    , m_mir_connection(nullptr)
    , m_configuration(nullptr)
{
    connect();
    if (isConnected()) {
        setConfiguration(
            mir_connection_create_display_configuration(m_mir_connection)
        );
    }
}

MirClientImpl::~MirClientImpl() {
    if (m_configuration)
        mir_display_config_release(m_configuration);
    if (m_mir_connection)
        mir_connection_release(m_mir_connection);
}

MirDisplayConfig* MirClientImpl::getConfiguration() const {
    return m_configuration;
}

bool MirClientImpl::isConnected() {
    return mir_connection_is_valid(m_mir_connection);
}

bool MirClientImpl::isConfigurationValid()
{
    return m_configuration != Q_NULLPTR;
}

void MirClientImpl::setConfiguration(MirDisplayConfig *conf) {
    if (m_configuration != conf) {
        m_configuration = conf;
        Q_EMIT configurationChanged();
    }
}

void MirClientImpl::applyConfiguration(MirDisplayConfig *conf) {
    mir_connection_confirm_base_display_configuration(
        m_mir_connection, conf
    );
}

void MirClientImpl::connect() {
    m_mir_connection = static_cast<MirConnection*>(
        QGuiApplication::platformNativeInterface()
            ->nativeResourceForIntegration("mirConnection")
    );
    if (!m_mir_connection)
        qWarning() << Q_FUNC_INFO << "Could not connect to Mir:";
    m_mir_connection = mir_connect_sync(NULL, "ubuntu-system-settings");
    if (m_mir_connection == nullptr || !isConnected()) {
        const char *error = "Unknown error";
        if (m_mir_connection != nullptr)
            error = mir_connection_get_error_message(m_mir_connection);
        qWarning() << Q_FUNC_INFO << "Could not connect to Mir:" << error;
    } else {
        qWarning() << Q_FUNC_INFO << "Connected successfully to mir.";
        mir_connection_set_display_config_change_callback(
            m_mir_connection, mir_display_change_callback, this);
        mir_connection_set_error_callback(m_mir_connection,
                                          mir_error_callback,
                                          this);
    }
}

QList<QSharedPointer<Output>> MirClientImpl::outputs()
{
    QList<QSharedPointer<Output>> list;
    int numOutputs = mir_display_config_get_num_outputs(m_configuration);
    for (int i = 0; i < numOutputs; i++) {
        auto mirOutput = mir_display_config_get_mutable_output(
            m_configuration, i
        );
        auto output = QSharedPointer<Output>(new MirOutputImpl(mirOutput));
        list.append(output);
    }
    return list;
}

void MirClientImpl::onConfigurationFailed(const QString &reason)
{
    Q_EMIT configurationFailed(reason);
}
} // DisplayPlugin
