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
    QObject(parent)
{
    QObject::connect(&m_accountsService,
                     SIGNAL (changed ()),
                     this,
                     SLOT (slotChanged()));
    updateUbuntuArt();
    updateCustomBackgrounds();
}

QString Background::getBackgroundFile()
{
    QVariant answer = m_accountsService.getUserProperty(
                "org.freedesktop.Accounts.User",
                "BackgroundFile");
    qCritical() << "getBackgroundFile";
    if (answer.isValid())
        qCritical() << "getBackgroundFile: was valid " << answer.toString();
        return answer.toString();

    return QString();
}

void Background::setBackgroundFile(QUrl backgroundFile)
{
    if (!backgroundFile.isLocalFile())
        return;

    if (backgroundFile.url() == m_backgroundFile)
        return;

    m_backgroundFile = backgroundFile.url();
    m_accountsService.customSetUserProperty("SetBackgroundFile",
                                            backgroundFile.path());
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
    return m_customBackgrounds;
}

void Background::updateCustomBackgrounds()
{
    m_customBackgrounds.clear();
    QFileInfoList tmpList;
    tmpList << getCustomBackgroundFolder().entryInfoList(QDir::Files | QDir::NoSymLinks);
    if (getCustomBackgroundFolder() != getContentHubFolder())
        tmpList << getContentHubFolder().entryInfoList(QDir::Files | QDir::NoSymLinks);
    if (!tmpList.isEmpty())
    {
        foreach (QFileInfo f, tmpList)
            m_customBackgrounds.append(QUrl::fromLocalFile(f.absoluteFilePath()).toString());
    }
    Q_EMIT customBackgroundsChanged();
}

QUrl Background::prepareBackgroundFile(const QUrl &url, bool shareWithGreeter)
{
    QUrl prepared = url;

    if (getCustomBackgroundFolder() != getContentHubFolder() &&
        url.path().startsWith(getContentHubFolder().path()))
    {
        QDir backgroundFolder = getCustomBackgroundFolder();
        QUrl newPath = QUrl::fromLocalFile(backgroundFolder.path() + "/" + url.fileName());

        if (QFile(newPath.path()).exists())
        {
            // The file already exists in the shared greeter data folder...
            // Likely we just pulled the same file from ContentHub again.
            // We don't want to show both versions in the picker grid, so just
            // promote it to greeter location so we still just have one copy.
            if (QFile(newPath.path()).remove())
                shareWithGreeter = true;
        }

        // Move file from local ContentHub dump to shared greeter data folder
        if (shareWithGreeter &&
            QDir::root().mkpath(backgroundFolder.path()) &&
            QFile::rename(url.path(), newPath.path()))
        {
            updateCustomBackgrounds();
            prepared = newPath;
        }
    }

    return prepared;
}

QDir Background::getCustomBackgroundFolder()
{
    // We want a location we can share with the greeter
    QString dataDir(qgetenv("XDG_GREETER_DATA_DIR"));
    if (dataDir.isEmpty())
        return getContentHubFolder();
    else
        return dataDir + "/ubuntu-system-settings/Pictures";
}

QDir Background::getContentHubFolder()
{
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/Pictures";
}

QStringList Background::ubuntuArt()
{
    return m_ubuntuArt;
}
void Background::updateUbuntuArt()
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

bool Background::fileExists(const QString &file)
{
    if (file.isEmpty() || file.isNull())
        return false;

    return QFile(file).exists();
}

void Background::rmFile(const QString &file)
{
    if (file.isEmpty() || file.isNull())
        return;

    if (!file.contains(getCustomBackgroundFolder().path()) && !file.contains(getContentHubFolder().path()))
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
