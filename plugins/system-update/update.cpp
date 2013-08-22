/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Didier Roche <didier.roche@canonical.com>
 *
*/

#include "update.h"
#include <QDebug>
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

// FIXME: need to do this better including #include "../../src/i18n.h"
// and linking to it
#include <libintl.h>
QString _(const char *text)
{
    return QString::fromUtf8(dgettext(0, text));
}

/**************/
/* WILL SPLIT */
class MyArrayWithDictInside
{
public:
    QVector< QMap<QString, QString> > foo;
};
Q_DECLARE_METATYPE(MyArrayWithDictInside)


QDBusArgument &operator<<(QDBusArgument &argument, const MyArrayWithDictInside)
{
 // you don't really need to implement this one if you don't plan writing it to the bus but i think qDbusRegisterType complains if you don't have it
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MyArrayWithDictInside &bar)
{
     while ( !argument.atEnd() ) {
         QMap<QString, QString> map;
         argument.beginMap();
         while ( !argument.atEnd() ) {
             QString key;
             QString value;
             argument.beginMapEntry();
             argument >> key >> value;
             argument.endMapEntry();
             map.insert( key, value );
         }

         argument.endMap();

         bar.foo.append(map);
     }

     argument.endArray();
     return argument;
}

/**************/

Update::Update(QObject *parent) :
    QObject(parent),
    m_infoMessage(""),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_SystemServiceIface ("com.canonical.SystemImage",
                         "/Service",
                         "com.canonical.SystemImage",
                         m_systemBusConnection)
{
    qRegisterMetaType<MyArrayWithDictInside>();
    qDBusRegisterMetaType<MyArrayWithDictInside>();

    // signals to forward directly to QML
    connect(&m_SystemServiceIface, SIGNAL(UpdateAvailableStatus(bool, bool, int, int, QString, MyArrayWithDictInside, QString)),
               this, SLOT(ProcessAvailableStatus(bool, bool, int, int, QString, MyArrayWithDictInside, QString)));

/*    connect(&m_SystemServiceIface, SIGNAL(UpdateAvailableStatus(bool, bool, int, int, QString, QDBusRawType::aa{ss}, QString)),
            this, SLOT(ProcessAvailableStatus(bool, bool, int, int, QString, const QDBusArgument&, QString)));*/
    connect(&m_SystemServiceIface, SIGNAL(UpdateProgress(int, double)),
                this, SIGNAL(updateProgress(int, double)));
    connect(&m_SystemServiceIface, SIGNAL(UpdatePaused(int)),
                this, SIGNAL(updatePaused(int)));
    connect(&m_SystemServiceIface, SIGNAL(UpdateDownloaded()),
                this, SIGNAL(updateDownloaded()));
    connect(&m_SystemServiceIface, SIGNAL(UpdateFailed(int, QString)),
                this, SIGNAL(updateFailed(int, QString)));

    this->CheckForUpdate();

}

Update::~Update() {
}

void Update::CheckForUpdate() {
    m_SystemServiceIface.call("CheckForUpdate");
}

void Update::DownloadUpdate() {
    m_SystemServiceIface.call("DownloadUpdate");
}

QString Update::ApplyUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("ApplyUpdate");
    if (reply.isValid())
        return reply.value();
    return _("Can't apply the current update (can't contact service)");
}

QString Update::CancelUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("Cancel");
    if (reply.isValid())
        return reply.value();
    return _("Can't cancel current request (can't contact service)");
}

QString Update::PauseUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("PauseUpdate");
    if (reply.isValid())
        return reply.value();
    return _("Can't pause current request (can't contact service)");
}

QString Update::InfoMessage() {
    return m_infoMessage;
}

void Update::SetInfoMessage(QString infoMessage) {
    m_infoMessage = infoMessage;
    Q_EMIT infoMessageChanged();
}

QString Update::TranslateFromBackend(QString msg) {
    // TODO: load in the backend context .mo file
    return msg;
}


// We'll care about that one once connected to the signal
void Update::ProcessAvailableStatus(bool, bool, int, int, QString, const QDBusArgument &, QString)
{
    //const QDBusArgument &bar = foo;
    //Q_EMIT updateAvailableStatus();
}

/*
    QDBusReply<qint64> reply2 = m_SystemServiceIface.call("GetUpdateSize");
    if (reply2.isValid())
        m_updateSize = QString("%1 Mb").arg(QString::number(reply2.value()/1024.0/1024.0));
    else
        m_updateSize = "Unknown";

    // TODO: descriptions (array of dict)

}
*/
