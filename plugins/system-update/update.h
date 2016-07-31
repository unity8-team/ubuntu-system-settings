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

#ifndef UPDATE_H
#define UPDATE_H

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QStringList>

namespace UpdatePlugin
{
class Update : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Kind kind READ kind
            WRITE setKind NOTIFY kindChanged)
    Q_PROPERTY(uint binaryFilesize READ binaryFilesize
            WRITE setBinaryFilesize NOTIFY binaryFilesizeChanged)
    Q_PROPERTY(QString changelog READ changelog
            WRITE setChangelog NOTIFY changelogChanged)
    Q_PROPERTY(QString channel READ channel
            WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt
            WRITE setCreatedAt NOTIFY createdAtChanged)
    Q_PROPERTY(bool installed READ installed
            WRITE setInstalled NOTIFY installedChanged)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt
            WRITE setUpdatedAt NOTIFY updatedAtChanged)
    Q_PROPERTY(QString downloadHash READ downloadHash
            WRITE setDownloadHash NOTIFY downloadHashChanged)
    Q_PROPERTY(QString downloadId READ downloadId
            WRITE setDownloadId NOTIFY downloadIdChanged)
    Q_PROPERTY(QString downloadUrl READ downloadUrl
            WRITE setDownloadUrl NOTIFY downloadUrlChanged)
    Q_PROPERTY(QString iconUrl READ iconUrl
            WRITE setIconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(QString identifier READ identifier
            WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(int progress READ progress
            WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(uint revision READ revision
            WRITE setRevision NOTIFY revisionChanged)
    Q_PROPERTY(State state READ state
            WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString signedDownloadUrl READ signedDownloadUrl
            WRITE setSignedDownloadUrl NOTIFY signedDownloadUrlChanged)
    Q_PROPERTY(QString title READ title
            WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString remoteVersion READ remoteVersion
            WRITE setRemoteVersion NOTIFY remoteVersionChanged)
    Q_PROPERTY(QString localVersion READ localVersion
            WRITE setLocalVersion NOTIFY localVersionChanged)
    Q_PROPERTY(QString token READ token NOTIFY tokenChanged)
    Q_PROPERTY(QStringList command READ command
               WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(bool automatic READ automatic WRITE setAutomatic
            NOTIFY automaticChanged)
    Q_PROPERTY(QString error READ error WRITE setError NOTIFY errorChanged)
    Q_PROPERTY(QString packageName READ packageName WRITE setPackageName
               NOTIFY packageNameChanged)
public:
    explicit Update(QObject *parent = nullptr) : QObject(parent) {};
    ~Update() {};

    enum class Kind : uint
    {
        KindUnknown = 1,
        KindClick = 2,
        KindImage = 4
    };

    enum class State : uint
    {
        StateUnknown,
        StateAvailable,
        StateUnavailable,
        StateQueuedForDownload,
        StateDownloading,
        StateDownloadingAutomatically,
        StateDownloadPaused,
        StateAutomaticDownloadPaused,
        StateInstalling,
        StateInstallingAutomatically,
        StateInstallPaused,
        StateInstallFinished,
        StateInstalled,
        StateDownloaded,
        StateFailed
    };

    Q_ENUMS(Kind State)

    static QString stateToString(const State &state);
    static State stringToState(const QString &state);

    static QString kindToString(const Kind &kind);
    static Kind stringToKind(const QString &kind);

    Kind kind() const;
    QString identifier() const;
    uint revision() const;
    uint binaryFilesize() const;
    QString changelog() const;
    QString channel() const;
    QDateTime createdAt() const;
    QDateTime updatedAt() const;
    QString downloadHash() const;
    QString downloadId() const;
    QString downloadUrl() const;
    QString iconUrl() const;
    bool installed() const;
    int progress() const;
    State state() const;
    QString signedDownloadUrl() const;
    QString title() const;
    QString remoteVersion() const;
    QString localVersion() const;
    QString token() const;
    QStringList command() const;
    bool automatic() const;
    QString error() const;
    QString packageName() const;

    void setKind(const Kind &kind);
    void setIdentifier(const QString &identifier);
    void setRevision(const uint &revision);
    void setBinaryFilesize(const uint &binaryFilesize);
    void setChangelog(const QString &changelog);
    void setChannel(const QString &channel);
    void setCreatedAt(const QDateTime &createdAt);
    void setUpdatedAt(const QDateTime &updatedAt);
    void setDownloadHash(const QString &downloadHash);
    void setDownloadId(const QString &token);
    void setDownloadUrl(const QString &downloadUrl);
    void setIconUrl(const QString &iconUrl);
    void setInstalled(const bool installed);
    void setProgress(const int &progress);
    void setState(const State &state);
    void setSignedDownloadUrl(const QString &signedDownloadUrl);
    void setTitle(const QString &title);
    void setRemoteVersion(const QString &version);
    void setLocalVersion(const QString &version);
    void setToken(const QString &token);
    void setCommand(const QStringList &command);
    void setAutomatic(const bool automatic);
    void setError(const QString &error);
    void setPackageName(const QString &error);

    bool isUpdateRequired();

    // Whether or not all fields in this update is equal to that of other.
    bool deepEquals(const Update *other) const;

    /* Whether or not either id and rev equals to other, or download id is set
    and that is equal to that of other. */
    bool operator==(const Update &other) const;
    bool equals(const Update &other) const;

Q_SIGNALS:
    void kindChanged();
    void identifierChanged();
    void revisionChanged();
    void binaryFilesizeChanged();
    void changelogChanged();
    void channelChanged();
    void createdAtChanged();
    void installedChanged();
    void updatedAtChanged();
    void downloadHashChanged();
    void downloadIdChanged();
    void downloadUrlChanged();
    void iconUrlChanged();
    void progressChanged();
    void stateChanged();
    void signedDownloadUrlChanged();
    void titleChanged();
    void remoteVersionChanged();
    void localVersionChanged();
    void tokenChanged();
    void commandChanged();
    void automaticChanged();
    void errorChanged();
    void packageNameChanged();
protected:
    Kind m_kind = Kind::KindUnknown;
    QString m_identifier = QString::null;
    uint m_revision = 0;
    uint m_binaryFilesize = 0;
    QString m_changelog = QString::null;
    QString m_channel = QString::null;
    QDateTime m_createdAt = QDateTime();
    QDateTime m_updatedAt = QDateTime();
    QString m_downloadHash = QString::null;
    QString m_downloadUrl = QString::null;
    QString m_error = QString::null;
    QString m_iconUrl = QString::null;
    bool m_installed = false;
    int m_progress = 0;
    State m_state = State::StateUnknown;
    QString m_signedDownloadUrl = QString::null;
    QString m_title = QString::null;
    QString m_localVersion = QString::null;
    QString m_remoteVersion = QString::null;
    QString m_token = QString::null;
    QString m_downloadId = QString::null;
    QStringList m_command = QStringList();
    bool m_automatic = false;
    QString m_packageName = QString::null;
};
} // UpdatePlugin

#endif // UPDATE_H
