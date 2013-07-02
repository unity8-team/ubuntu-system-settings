#include "background.h"
#include <QDebug>
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

Background::Background(QObject *parent) :
    QObject(parent),
    system_bus_connection (QDBusConnection::systemBus()),
    accountsservice_iface ("org.freedesktop.Accounts",
                           "/org/freedesktop/Accounts",
                           "org.freedesktop.Accounts",
                            system_bus_connection)
{
    if (!accountsservice_iface.isValid()) {
        return;
    }

    background_file = get_background_file();

    QDBusReply<QDBusObjectPath> q_object_path = accountsservice_iface.call(
                "FindUserById", qlonglong(getuid()));

    if (q_object_path.isValid()) {
        object_path = q_object_path.value().path();
    }

    system_bus_connection.connect("org.freedesktop.Accounts",
                                  object_path,
                                  "org.freedesktop.Accounts.User",
                                  "Changed",
                                  this,
                                  SLOT(slotChanged()));

    background_file = get_background_file();
}

QString Background::get_background_file()
{
    QDBusInterface user_interface (
                "org.freedesktop.Accounts",
                object_path,
                "org.freedesktop.Accounts.User",
                system_bus_connection,
                this);

    if (user_interface.isValid()) {
        return user_interface.property("BackgroundFile").toString();
    }

    return NULL;
}

void Background::slotChanged()
{
    QString new_background = get_background_file();
    if (new_background != background_file) {
        qDebug() << "Background changed to: " << new_background;
        background_file = new_background;
        Q_EMIT backgroundFileChanged();
    }
}

QString Background::backgroundFile()
{
     return background_file;
}

Background::~Background() {
}
