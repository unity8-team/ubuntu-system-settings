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
 * Iain Lane <iain.lane@canonical.com>
 *
*/

#include "background.h"
#include <QDir>
#include <QStandardPaths>
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>
#include <QDebug>

Background::Background(QObject *parent) :
    QObject(parent),
    //m_ubuntuArt(0),
    //m_customBackgrounds(0),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_accountsserviceIface ("org.freedesktop.Accounts",
                            "/org/freedesktop/Accounts",
                            "org.freedesktop.Accounts",
                             m_systemBusConnection)
{
    if (!m_accountsserviceIface.isValid()) {
        return;
    }

    QDBusReply<QDBusObjectPath> qObjectPath = m_accountsserviceIface.call(
                "FindUserById", qlonglong(getuid()));

    if (qObjectPath.isValid()) {
        m_objectPath = qObjectPath.value().path();
    }

    m_systemBusConnection.connect("org.freedesktop.Accounts",
                                  m_objectPath,
                                  "org.freedesktop.Accounts.User",
                                  "Changed",
                                  this,
                                  SLOT(slotChanged()));
}

QString Background::getBackgroundFile()
{
    QDBusInterface userInterface (
                "org.freedesktop.Accounts",
                m_objectPath,
                "org.freedesktop.DBus.Properties",
                m_systemBusConnection,
                this);

    if (userInterface.isValid()) {
        QDBusReply<QDBusVariant> answer = userInterface.call (
                    "Get",
                    "org.freedesktop.Accounts.User",
                    "BackgroundFile");

        if (answer.isValid())
            return answer.value().variant().toString();
    }

    return QString();
}

void Background::setBackgroundFile(QUrl backgroundFile)
{
    if (!backgroundFile.isLocalFile())
        return;

    if (backgroundFile.url() == m_backgroundFile)
        return;

    QDBusInterface userInterface (
                "org.freedesktop.Accounts",
                m_objectPath,
                "org.freedesktop.Accounts.User",
                m_systemBusConnection,
                this);

    if (!userInterface.isValid())
        return;

    m_backgroundFile = backgroundFile.url();
    userInterface.call("SetBackgroundFile", backgroundFile.path());
    Q_EMIT backgroundFileChanged();
}

void Background::slotChanged()
{
    QString new_background = QUrl::fromLocalFile(getBackgroundFile()).url();
    if (new_background != m_backgroundFile) {
        m_backgroundFile = new_background;
        Q_EMIT backgroundFileChanged();
    }
}

QString Background::backgroundFile()
{
    if (m_backgroundFile.isEmpty() || m_backgroundFile.isNull())
        m_backgroundFile = QUrl::fromLocalFile(getBackgroundFile()).url();

     return m_backgroundFile;
}

QStringList Background::customBackgrounds()
{
    if (m_customBackgrounds.isEmpty())
    {
        updateCustomBackgrounds();
    }
    return m_customBackgrounds;
}

void Background::updateCustomBackgrounds()
{
    m_customBackgrounds.clear();
    QString customPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/Pictures";
    QDir dir(customPath);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList tmpList = dir.entryInfoList();
    foreach (QFileInfo f, tmpList)
    {
        m_customBackgrounds.append(QUrl::fromLocalFile(f.absoluteFilePath()).toString());
    }
    Q_EMIT customBackgroundsChanged();
}

QStringList Background::ubuntuArt()
{
    if (m_ubuntuArt.isEmpty())
    {
        QDir dir("/usr/share/backgrounds/");
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList tmpList = dir.entryInfoList();
        foreach (QFileInfo f, tmpList)
        {
            if (f.fileName() != "warty-final-ubuntu.png")
                m_ubuntuArt.append(QUrl::fromLocalFile(f.absoluteFilePath()).toString());
        }
        Q_EMIT ubuntuArtChanged();
    }
    return m_ubuntuArt;
}

void Background::rmFile(const QString &file)
{
    if (file.isEmpty() || file.isNull())
        return;

    if (!file.contains(QStandardPaths::writableLocation(QStandardPaths::DataLocation)))
        return;

    QUrl fileUri(file);
    if (!fileUri.isLocalFile())
        return;

    QFile filePath(fileUri.path());
    if (filePath.exists())
    {
        if (filePath.remove())
            updateCustomBackgrounds();
    }
}

Background::~Background() {
}
