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

    bool systemUpdate() const { return m_systemUpdate; }
    QString getPackageName() const { return m_packagename; }
    QString getTitle() const { return m_title; }
    QString getLocalVersion() const { return m_local_version; }
    QString getRemoteVersion() const { return m_remote_version; }
    QString iconUrl() const { return m_icon_url; }
    QString lastUpdateDate() const { return m_lastUpdateDate; }
    int binaryFilesize() const { return m_binary_filesize; }
    int downloadProgress() const { return m_download_progress; }
    bool updateRequired() const { return m_update; }
    bool updateState() const { return m_update_state; }
    bool updateReady() const { return m_update_ready; }
    bool selected() const { return m_selected; }
    QString getError() const { return m_error; }
    const QString& getClickUrl() const { return m_click_url; }
    QString downloadUrl() const { return m_downloadUrl; }
    QString clickToken() const { return m_clickToken; }

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
    void setUpdateAvailable(bool available);
    void setLastUpdateDate(const QString date);
    void setClickUrl(const QString &url);
    void setDownloadUrl(const QString &url);
    void setClickToken(const QString &token);

protected:
    int m_binary_filesize;
    QString m_click_url;
    QString m_clickToken;
    QString m_downloadUrl;
    int m_download_progress;
    QString m_error;
    QString m_icon_url;
    QString m_lastUpdateDate;
    QString m_local_version;
    QString m_packagename;
    QString m_remote_version;
    bool m_selected;
    bool m_systemUpdate;
    QString m_title;
    bool m_update;
    bool m_update_ready;
    bool m_update_state;

    bool getIgnoreUpdates();
};

}

#endif // UPDATE_H
