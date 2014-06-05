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

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "accountsservice.h"

#include <QDBusInterface>
#include <QDir>
#include <QObject>
#include <QProcess>
#include <QUrl>

class Background : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString backgroundFile
                READ backgroundFile
                WRITE setBackgroundFile
                NOTIFY backgroundFileChanged )

    Q_PROPERTY( QStringList customBackgrounds
                READ customBackgrounds
                NOTIFY customBackgroundsChanged )

    Q_PROPERTY( QStringList ubuntuArt
                READ ubuntuArt
                NOTIFY ubuntuArtChanged )
    
public:
    explicit Background(QObject *parent = 0);
    ~Background();
    QString backgroundFile();
    void setBackgroundFile(QUrl backgroundFile);
    Q_INVOKABLE QUrl prepareBackgroundFile(const QUrl &url);
    Q_INVOKABLE bool fileExists(const QString &file);
    Q_INVOKABLE void rmFile(const QString &file);
    QStringList customBackgrounds();
    QStringList ubuntuArt();

public Q_SLOTS:
    void slotChanged();

Q_SIGNALS:
    void backgroundFileChanged();
    void customBackgroundsChanged();
    void ubuntuArtChanged();

private:
    AccountsService m_accountsService;
    QStringList m_ubuntuArt;
    QStringList m_customBackgrounds;
    void updateCustomBackgrounds();
    void updateUbuntuArt();
    QString m_backgroundFile;
    QString getBackgroundFile();
    QDir getCustomBackgroundFolder();
    QDir getContentHubFolder();
};

#endif // BACKGROUND_H
