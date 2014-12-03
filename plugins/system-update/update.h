/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Diego Sarmentero <diego.sarmentero@canonical.com>
 *
*/

#ifndef UPDATE_H
#define UPDATE_H

#include <QObject>
#include <QtQml>
#include <QString>
#include <QStringList>

namespace UpdatePlugin {

class Update : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool systemUpdate READ systemUpdate WRITE setSystemUpdate
               NOTIFY systemUpdateChanged)
    Q_PROPERTY(QString packageName READ getPackageName NOTIFY packageNameChanged)
    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString localVersion READ getLocalVersion
               NOTIFY localVersionChanged)
    Q_PROPERTY(QString remoteVersion READ getRemoteVersion
               NOTIFY remoteVersionChanged)
    Q_PROPERTY(bool updateRequired READ updateRequired WRITE setUpdateRequired
               NOTIFY updateRequiredChanged)
    Q_PROPERTY(QString iconUrl READ iconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(int binaryFilesize READ binaryFilesize
               NOTIFY binaryFilesizeChanged)
    Q_PROPERTY(bool updateState READ updateState WRITE setUpdateState
               NOTIFY updateStateChanged)
    Q_PROPERTY(bool updateReady READ updateReady WRITE setUpdateReady
               NOTIFY updateReadyChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected
               NOTIFY selectedChanged)
    Q_PROPERTY(QString error READ getError NOTIFY errorChanged)
    Q_PROPERTY(QString lastUpdateDate READ lastUpdateDate
               NOTIFY lastUpdateDateChanged)
    Q_PROPERTY(int downloadProgress READ downloadProgress
               NOTIFY downloadProgressChanged)
    Q_PROPERTY(QString downloadUrl READ downloadUrl NOTIFY downloadUrlChanged)
    Q_PROPERTY(QString clickToken READ clickToken NOTIFY clickTokenChanged)

Q_SIGNALS:
    void systemUpdateChanged();
    void titleChanged();
    void binaryFilesizeChanged();
    void iconUrlChanged();
    void localVersionChanged();
    void remoteVersionChanged();
    void updateRequiredChanged();
    void updateStateChanged();
    void updateReadyChanged();
    void selectedChanged();
    void errorChanged();
    void downloadProgressChanged();
    void lastUpdateDateChanged();
    void downloadUrlChanged();
    void clickTokenChanged();
    void packageNameChanged();

public:
    explicit Update(QObject *parent = 0);
    virtual ~Update();

    bool systemUpdate() { return m_systemUpdate; }
    QString getPackageName() { return m_packagename; }
    QString getTitle() { return m_title; }
    QString getLocalVersion() { return m_local_version; }
    QString getRemoteVersion() { return m_remote_version; }
    QString iconUrl() { return m_icon_url; }
    QString lastUpdateDate() { return m_lastUpdateDate; }
    int binaryFilesize() { return m_binary_filesize; }
    int downloadProgress() { return m_download_progress; }
    bool updateRequired() { return m_update; }
    bool updateState() { return m_update_state; }
    bool updateReady() { return m_update_ready; }
    bool selected() { return m_selected; }
    QString getError() { return m_error; }
    const QString& getClickUrl() const { return m_click_url; }
    QString downloadUrl() { return m_downloadUrl; }
    QString clickToken() { return m_clickToken; }

    void setSystemUpdate(bool isSystem);
    void initializeApplication(QString packagename, QString title,
                               QString version);
    void setRemoteVersion(QString &version);
    void setUpdateRequired(bool state);
    void setUpdateState(bool state);
    void setUpdateReady(bool ready);
    void setSelected(bool value);
    void setBinaryFilesize(int size);
    void setDownloadProgress(int progress);
    void setIconUrl(QString icon);
    void setError(QString error);
    void setUpdateAvailable(bool available) { m_update = available; }
    void setLastUpdateDate(const QString date);
    void setClickUrl(const QString &url) { m_click_url = url; }
    void setDownloadUrl(const QString &url);
    void setClickToken(const QString &token) { m_clickToken = token; Q_EMIT clickTokenChanged(); }

private:
    int m_binary_filesize = 0;
    QString m_click_url = QString::null;
    QString m_clickToken = QString::null;
    QString m_downloadUrl = QString::null;
    int m_download_progress = 0;
    QString m_error = QString::null;
    QString m_icon_url = QString::null;
    QString m_lastUpdateDate = QString::null;
    QString m_local_version = QString::null;
    QString m_packagename = QString::null;
    QString m_remote_version = QString::null;
    bool m_selected = false;
    bool m_systemUpdate = false;
    QString m_title = QString::null;
    bool m_update = false;
    bool m_update_ready = false;
    bool m_update_state = false;

    bool getIgnoreUpdates();
};

}

#endif // UPDATE_H
