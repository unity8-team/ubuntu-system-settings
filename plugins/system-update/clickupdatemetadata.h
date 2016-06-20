/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLICK_UPDATE_METADATA_H
#define CLICK_UPDATE_METADATA_H

#include "clickapiclient.h"

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin
{
class ClickUpdateMetadata : public ClickApiClient
{
    Q_OBJECT
    Q_PROPERTY(QString anonDownloadUrl READ anonDownloadUrl
            WRITE setAnonDownloadUrl NOTIFY anonDownloadUrlChanged)
    Q_PROPERTY(uint binaryFilesize READ binaryFilesize
            WRITE setBinaryFilesize NOTIFY binaryFilesizeChanged)
    Q_PROPERTY(QString changelog READ changelog
            WRITE setChangelog NOTIFY changelogChanged)
    Q_PROPERTY(QString channel READ channel
            WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(QString content READ content
            WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QStringList department READ department
            WRITE setDepartment NOTIFY departmentChanged)
    Q_PROPERTY(QString downloadSha512 READ downloadSha512
            WRITE setDownloadSha512 NOTIFY downloadSha512Changed)
    Q_PROPERTY(QString downloadUrl READ downloadUrl
            WRITE setDownloadUrl NOTIFY downloadUrlChanged)
    Q_PROPERTY(QString iconUrl READ iconUrl
            WRITE setIconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(QString name READ name
            WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString origin READ origin
            WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(QString packageName READ packageName
            WRITE setPackageName NOTIFY packageNameChanged)
    Q_PROPERTY(int revision READ revision
            WRITE setRevision NOTIFY revisionChanged)
    Q_PROPERTY(QString status READ status
            WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString title READ title
            WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString remoteVersion READ remoteVersion
            WRITE setRemoteVersion NOTIFY remoteVersionChanged)
    Q_PROPERTY(QString localVersion READ localVersion
            WRITE setLocalVersion NOTIFY localVersionChanged)
    Q_PROPERTY(QString clickToken READ clickToken NOTIFY clickTokenChanged)
    Q_PROPERTY(QStringList command READ command
               WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(bool automatic READ automatic WRITE setAutomatic
            NOTIFY automaticChanged)
public:
    explicit ClickUpdateMetadata(QObject *parent = 0);
    ~ClickUpdateMetadata();

    QString anonDownloadUrl() const;
    uint binaryFilesize() const;
    QString changelog() const;
    QString channel() const;
    QString content() const;
    QStringList department() const;
    QString downloadSha512() const;
    QString downloadUrl() const;
    QString iconUrl() const;
    QString name() const;
    QString origin() const;
    QString packageName() const;
    int revision() const;
    QString status() const;
    QString title() const;
    QString remoteVersion() const;
    QString localVersion() const;
    QString clickToken() const;
    QStringList command() const;
    bool automatic() const;

    void setAnonDownloadUrl(const QString &anonDownloadUrl);
    void setBinaryFilesize(const uint &binaryFilesize);
    void setChangelog(const QString &changelog);
    void setChannel(const QString &channel);
    void setContent(const QString &content);
    void setDepartment(const QStringList &department);
    void setDownloadSha512(const QString &downloadSha512);
    void setDownloadUrl(const QString &downloadUrl);
    void setIconUrl(const QString &iconUrl);
    void setName(const QString &name);
    void setOrigin(const QString &origin);
    void setPackageName(const QString &packageName);
    void setRevision(const int &revision);
    void setStatus(const QString &status);
    void setTitle(const QString &title);
    void setRemoteVersion(const QString &version);
    void setLocalVersion(const QString &version);
    void setClickToken(const QString &clickToken);
    void setCommand(const QStringList &command);
    void setAutomatic(const bool automatic);

    void requestClickToken();
    bool isUpdateRequired();

protected slots:
    void requestSucceeded(QNetworkReply *reply);
//     void tokenRequestSslFailed(const QList<QSslError> &errors);
//     void tokenRequestFailed(const QNetworkReply::NetworkError &code);
//     void tokenRequestSucceeded(const QNetworkReply* reply);

signals:
    void anonDownloadUrlChanged();
    void binaryFilesizeChanged();
    void changelogChanged();
    void channelChanged();
    void contentChanged();
    void departmentChanged();
    void downloadSha512Changed();
    void downloadUrlChanged();
    void iconUrlChanged();
    void nameChanged();
    void originChanged();
    void packageNameChanged();
    void revisionChanged();
    void sequenceChanged();
    void statusChanged();
    void titleChanged();
    void remoteVersionChanged();
    void localVersionChanged();
    void clickTokenChanged();
    void commandChanged();
    void automaticChanged();

    void clickTokenRequestSucceeded(const ClickUpdateMetadata *meta);
    void clickTokenRequestFailed(const ClickUpdateMetadata *meta);

private:
    QString m_anonDownloadUrl;
    uint m_binaryFilesize;
    QString m_changelog;
    QString m_channel;
    QString m_content;
    QStringList m_department;
    QString m_downloadSha512;
    QString m_downloadUrl;
    QString m_iconUrl;
    QString m_name;
    QString m_origin;
    QString m_packageName;
    int m_revision;
    QString m_status;
    QString m_title;
    QString m_localVersion;
    QString m_remoteVersion;
    QString m_clickToken;
    QStringList m_command;
    bool m_automatic;
};
} // UpdatePlugin

#endif // CLICK_UPDATE_METADATA_H
