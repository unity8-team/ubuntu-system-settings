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

typedef QMap<QString, QString> StringMap;
typedef QList<StringMap> ArrayDict;

Q_DECLARE_METATYPE(StringMap)
Q_DECLARE_METATYPE(ArrayDict)

class Update : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY( QString infoMessage
                READ InfoMessage
                WRITE SetInfoMessage
                NOTIFY infoMessageChanged)
    Q_PROPERTY( int downloadMode
                READ DownloadMode
                WRITE SetDownloadMode
                NOTIFY downloadModeChanged)

public:
    explicit Update(QObject *parent = 0);
    ~Update();

    enum State { CheckingError, Checking, NoUpdate, UpdateAvailable, DownloadRequested, Downloading, Paused, ReadyToInstall, DownloadFailed };

    QString InfoMessage();
    void SetInfoMessage(QString);
    int DownloadMode();
    void SetDownloadMode(int);

    Q_INVOKABLE void CheckForUpdate();
    Q_INVOKABLE void DownloadUpdate();
    Q_INVOKABLE QString ApplyUpdate();
    Q_INVOKABLE QString CancelUpdate();
    Q_INVOKABLE QString PauseDownload();
    Q_INVOKABLE QString TranslateFromBackend(QString);

public Q_SLOTS:
    void ProcessAvailableStatus(bool, bool, QString, int, QString, QList<QMap<QString, QString> >, QString);
    void ProcessSettingChanged(QString, QString);

Q_SIGNALS:
    void updateAvailableStatus(bool isAvailable, bool downloading, QString availableVersion, int updateSize,
                               QString lastUpdateDate, QStringList descriptions, QString errorReason);
    void updateProgress(int percentage, double eta);
    void updatePaused(int percentage);
    void updateDownloaded();
    void updateFailed(int consecutiveFailureCount, QString lastReason);
    void infoMessageChanged();
    void downloadModeChanged();


private:
    QString m_infoMessage;
    int m_downloadMode;

    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_SystemServiceIface;
};

#endif // UPDATE_H
