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

Update::Update(QObject *parent) :
    QObject(parent),
    m_infoMessage(""),
    m_downloadMode(-1),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_SystemServiceIface ("com.canonical.SystemImage",
                         "/Service",
                         "com.canonical.SystemImage",
                         m_systemBusConnection)
{

    // register types
    qDBusRegisterMetaType<StringMap>();
    qDBusRegisterMetaType<ArrayDict>();

    connect(&m_SystemServiceIface, SIGNAL(UpdateAvailableStatus(bool, bool, QString, int, QString, QList<QMap<QString, QString> >, QString)),
               this, SLOT(ProcessAvailableStatus(bool, bool, QString, int, QString, ArrayDict, QString)));
    // signals to forward directly to QML
    connect(&m_SystemServiceIface, SIGNAL(UpdateProgress(int, double)),
                this, SIGNAL(updateProgress(int, double)));
    connect(&m_SystemServiceIface, SIGNAL(UpdatePaused(int)),
                this, SIGNAL(updatePaused(int)));
    connect(&m_SystemServiceIface, SIGNAL(UpdateDownloaded()),
                this, SIGNAL(updateDownloaded()));
    connect(&m_SystemServiceIface, SIGNAL(UpdateFailed(int, QString)),
                this, SIGNAL(updateFailed(int, QString)));
    connect(&m_SystemServiceIface, SIGNAL(SettingChanged(QString, QString)),
                this, SLOT(ProcessSettingChanged(QString, QString)));

    this->CheckForUpdate();

}

Update::~Update() {
}

void Update::CheckForUpdate() {
    m_SystemServiceIface.asyncCall("CheckForUpdate");
}

void Update::DownloadUpdate() {
    m_SystemServiceIface.asyncCall("DownloadUpdate");
}

QString Update::ApplyUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("ApplyUpdate");
    if (reply.isValid())
        return reply.value();
    return _("Can't apply the current update (can't contact service)");
}

QString Update::CancelUpdate() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("CancelUpdate");
    if (reply.isValid())
        return reply.value();
    return _("Can't cancel current request (can't contact service)");
}

QString Update::PauseDownload() {
    QDBusReply<QString> reply = m_SystemServiceIface.call("PauseDownload");
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

int Update::DownloadMode() {
    if (m_downloadMode != -1)
        return m_downloadMode;

    QDBusReply<QString> reply = m_SystemServiceIface.call("GetSetting", "auto_download");
    int default_mode = 1;
    if (reply.isValid()) {
        bool ok;
        int result;
        result = reply.value().toInt(&ok);
        if (ok)
            m_downloadMode = result;
        else
            m_downloadMode = default_mode;
    }
    else
        m_downloadMode = default_mode;
    return m_downloadMode;
}

void Update::SetDownloadMode(int value) {
    if (m_downloadMode == value)
        return;

    m_downloadMode = value;
    m_SystemServiceIface.asyncCall("SetSetting", "auto_download", QString::number(value));
}

void Update::ProcessSettingChanged(QString key, QString newvalue) {
    if(key == "auto_download") {
        bool ok;
        int newintValue;
        newintValue = newvalue.toInt(&ok);
        if (ok) {
            m_downloadMode = newintValue;
            Q_EMIT downloadModeChanged();
        }
    }
}

QString Update::TranslateFromBackend(QString msg) {
    // TODO: load in the backend context .mo file
    return msg;
}

void Update::ProcessAvailableStatus(bool isAvailable, bool downloading, QString availableVersion, int updateSize, QString lastUpdateDate, QList<QMap<QString, QString> > desc, QString errorReason)
{
    //TODO: bind with the real description once we can listen to the signal
    desc = desc;
    QStringList descriptions;
    descriptions.append("Description for update1");
    descriptions.append("Description for update2");
    descriptions.append("Description for update3");

    Q_EMIT updateAvailableStatus(isAvailable, downloading, availableVersion, updateSize, lastUpdateDate, descriptions, errorReason);
}

