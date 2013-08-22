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

#ifndef UPDATE_H
#define UPDATE_H

#include <QtDBus>
#include <QDBusInterface>
#include <QObject>
#include <QProcess>
#include <QUrl>

class Update : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY( QString infoMessage
                READ InfoMessage
                WRITE SetInfoMessage
                NOTIFY infoMessageChanged)
    
public:
    explicit Update(QObject *parent = 0);
    ~Update();

    enum State { CheckingError, Checking, NoUpdate, UpdateAvailable, Downloading, Paused, ReadyToInstall, DownloadFailed };

    QString InfoMessage();
    void SetInfoMessage(QString);

    Q_INVOKABLE void CheckForUpdate();
    Q_INVOKABLE void DownloadUpdate();
    Q_INVOKABLE QString ApplyUpdate();
    Q_INVOKABLE QString CancelUpdate();
    Q_INVOKABLE QString PauseUpdate();
    Q_INVOKABLE QString TranslateFromBackend(QString);



public Q_SLOTS:
    void ProcessAvailableStatus(bool, bool, int, int, QString, QString);

Q_SIGNALS:
    void updateAvailableStatus(bool, bool, int, int, QString, QList< QMap<QString, QString> >, QString);
    void updateProgress(int percentage, double eta);
    void updatePaused(int percentage);
    void updateDownloaded();
    void updateFailed(int consecutiveFailureCount, QString lastReason);
    void infoMessageChanged();

private:
    QString m_infoMessage;

    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_SystemServiceIface;

    void m_getUpdateInfos();
};

#endif // UPDATE_H
