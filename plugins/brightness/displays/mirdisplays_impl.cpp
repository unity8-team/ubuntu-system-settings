#include "mirdisplays_impl.h"

#include <QDebug>
#include <QQmlEngine>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>


static void mir_display_change_callback(MirConnection *connection, void *context) {
    qWarning() << "mir_display_change_callback" << context;
    MirDisplayConfiguration *conf = mir_connection_create_display_config(
            connection);
    static_cast<MirDisplaysImpl*>(context)->setConfiguration(conf);

}

MirDisplaysImpl::MirDisplaysImpl(QObject *parent)
        : MirDisplays(parent)
        , m_mir_connection(nullptr)
        , m_configuration(nullptr)
{
    connect();
    if (isConnected()) {
        setConfiguration(
            // deprecated, use mir_connection_create_display_configuration
            mir_connection_create_display_config(m_mir_connection)
        );
    }
}

MirDisplaysImpl::~MirDisplaysImpl() {
    mir_display_config_destroy(m_configuration);
}

MirDisplayConfiguration* MirDisplaysImpl::getConfiguration() const {
    return m_configuration;
}

bool MirDisplaysImpl::isConnected() {
    return mir_connection_is_valid(m_mir_connection);
}

void MirDisplaysImpl::setConfiguration(MirDisplayConfiguration * conf) {
    m_configuration = conf;
}

void MirDisplaysImpl::applyConfiguration(MirDisplayConfiguration * conf) {
    mir_wait_for(mir_connection_apply_display_config(m_mir_connection, conf));
    qWarning() << "mirdisplays applyConfiguration" << conf;

    const char *error = "No error";
    error = mir_connection_get_error_message(m_mir_connection);
    qWarning() << "Mir apply configuration error:" << error;
}

void MirDisplaysImpl::connect() {
    qWarning() << "Connecting...";
    m_mir_connection = static_cast<MirConnection*>(
            QGuiApplication::platformNativeInterface()
                ->nativeResourceForIntegration("mirConnection")
    );
    if (m_mir_connection == nullptr || !isConnected()) {
        const char *error = "Unknown error";
        if (m_mir_connection != nullptr)
            error = mir_connection_get_error_message(m_mir_connection);
        qWarning() << "Could not connect to Mir:" << error;
    } else {
        qWarning() << "Using Mir as display server.";
        mir_connection_set_display_config_change_callback(
                m_mir_connection, mir_display_change_callback, this);
    }
}
