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

#include <QDBusInterface>
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
    
public:
    explicit Background(QObject *parent = 0);
    ~Background();
    QString backgroundFile();
    void setBackgroundFile(QUrl backgroundFile);
    Q_INVOKABLE QStringList listUbuntuArt(const QString &dirString);
    Q_INVOKABLE void rmFile(const QString &file);
    QStringList customBackgrounds();


public Q_SLOTS:
    void slotChanged();

Q_SIGNALS:
    void backgroundFileChanged();
    void customBackgroundsChanged();


private:
    QStringList m_ubuntuArtList;
    QStringList m_customBackgrounds;
    void updateCustomBackgrounds();
    QString m_backgroundFile;
    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_accountsserviceIface;
    QString getBackgroundFile();
};

#endif // BACKGROUND_H
