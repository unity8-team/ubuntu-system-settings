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

#define SYSTEM_BACKGROUND_DIR "/usr/share/backgrounds"

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
    QString filename = answer.toString();

    if (filename.isEmpty() || !QFile::exists(filename))
        return defaultBackgroundFile();
    else
        return filename;
}

void Background::setBackgroundFile(QUrl backgroundFile)
{
    if (!backgroundFile.isLocalFile())
        return;

    if (backgroundFile.url() == m_backgroundFile)
        return;

    QString oldBackgroundFile = m_backgroundFile;

    m_backgroundFile = backgroundFile.url();
    m_accountsService.customSetUserProperty("SetBackgroundFile",
                                            backgroundFile.path());
    Q_EMIT backgroundFileChanged();

    // If old background was a system copy that we still have on the system,
    // delete our copy.  We don't need it anymore.
    if (oldBackgroundFile.contains(getCopiedSystemBackgroundFolder().path())) {
        QString fileName = QUrl(oldBackgroundFile).fileName();
        if (QFile::exists(SYSTEM_BACKGROUND_DIR "/" + fileName)) {
            rmFile(oldBackgroundFile);
        }
    }
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
    if (getCustomBackgroundFolder() != getContentHubFolder()) {
        tmpList << getContentHubFolder().entryInfoList(QDir::Files | QDir::NoSymLinks);

        // If any of our copied system backgrounds are no longer on the system,
        // treat them as custom backgrounds.  This would only likely happen if
        // a user was using a background from vivid-wallpapers and we upgraded
        // to xenial-wallpapers.  In this case, we want the old paper to show
        // as a custom background, so that the user can delete it when done
        // with it.
        // So scan the copied backgrounds.
        QFileInfoList copyList = getCopiedSystemBackgroundFolder().entryInfoList(QDir::Files | QDir::NoSymLinks);
        QDir systemDir = QDir(SYSTEM_BACKGROUND_DIR);
        foreach (QFileInfo f, copyList) {
            if (!systemDir.exists(f.fileName()))
                tmpList << f;
        }
    }
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
        !url.path().startsWith(getCustomBackgroundFolder().path()) &&
        url != QUrl::fromLocalFile(defaultBackgroundFile()))
    {
        QDir backgroundFolder;
        bool moveFile = false;
        if (url.path().startsWith(getContentHubFolder().path())) {
            backgroundFolder = getCustomBackgroundFolder();
            moveFile = true;
        } else {
            backgroundFolder = getCopiedSystemBackgroundFolder();
        }

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
            QDir::root().mkpath(backgroundFolder.path()))
        {
            if ((moveFile && QFile::rename(url.path(), newPath.path())) ||
                (!moveFile && (link(url.path().toUtf8().data(),
                                     newPath.path().toUtf8().data()) == 0 ||
                               QFile::copy(url.path(), newPath.path()))))
            {
                updateUbuntuArt();
                updateCustomBackgrounds();
                prepared = newPath;
            }
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

QDir Background::getCopiedSystemBackgroundFolder()
{
    return getCustomBackgroundFolder().path() + "/System";
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
    QString envDir(qgetenv("SYSTEM_SETTINGS_UBUNTU_ART_DIR"));
    QDir dir;
    QDir copiedBackgroundDir;

    if (envDir != "") {
        dir = QDir(envDir);
        copiedBackgroundDir = dir;
    } else {
        dir = QDir(SYSTEM_BACKGROUND_DIR);
        copiedBackgroundDir = getCopiedSystemBackgroundFolder();
    }

    m_ubuntuArt.clear();

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QFileInfoList tmpList = dir.entryInfoList();
    foreach (QFileInfo f, tmpList)
    {
        QString absPath = f.absoluteFilePath();

        // Prefer copied versions.
        if (copiedBackgroundDir.exists(f.fileName()))
            absPath = copiedBackgroundDir.absoluteFilePath(f.fileName());
        
        m_ubuntuArt.append(QUrl::fromLocalFile(absPath).toString());
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

    if (!file.contains(getCustomBackgroundFolder().path()) &&
        !file.contains(getCopiedSystemBackgroundFolder().path()) &&
        !file.contains(getContentHubFolder().path()))
        return;

    QUrl fileUri(file);
    if (!fileUri.isLocalFile())
        return;

    QFile filePath(fileUri.path());
    if (filePath.exists())
    {
        if (filePath.remove()) {
            updateUbuntuArt();
            updateCustomBackgrounds();
        }
    }
}

QString Background::defaultBackgroundFile() const
{
    /* TODO: For now hardcoded path, later we'll use GSettings */
    return SYSTEM_BACKGROUND_DIR "/warty-final-ubuntu.png";
}

Background::~Background() {
}
