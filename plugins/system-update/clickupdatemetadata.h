/*
 * Copyright (C) 2013-2016 Canonical Ltd
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
 * Class that represents metadata on a click update. This class will talk
 * to the click servers in order to sign the download URL as soon as it is
 * created.
 *
*/
#ifndef CLICKMETA_H
#define CLICKMETA_H

namespace UpdatePlugin {

class ClickUpdateMetadata : public QObject
{
    Q_OBJECT
public:
    explicit ClickUpdateMetadata(QObject *parent = 0, UbuntuOne::Token token, QJsonDocument metadata);
    ~ClickUpdateMetadata() {}
    Q_PROPERTY(QString anonDownloadUrl READ anonDownloadUrl NOTIFY anonDownloadUrlChanged)
    Q_PROPERTY(uint binaryFilesize READ binaryFilesize NOTIFY binaryFilesizeChanged)
    Q_PROPERTY(QString changelog READ changelog NOTIFY changelogChanged)
    Q_PROPERTY(QString channel READ channel NOTIFY channelChanged)
    Q_PROPERTY(QString content READ content NOTIFY contentChanged)
    Q_PROPERTY(QStringList department READ department NOTIFY departmentChanged)
    Q_PROPERTY(QString downloadSha512 READ downloadSha512 NOTIFY downloadSha512Changed)
    Q_PROPERTY(QString downloadUrl READ downloadUrl NOTIFY downloadUrlChanged)
    Q_PROPERTY(QString iconUrl READ iconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString origin READ origin NOTIFY originChanged)
    Q_PROPERTY(QString packageName READ packageName NOTIFY packageNameChanged)
    Q_PROPERTY(int revision READ revision NOTIFY revisionChanged)
    Q_PROPERTY(int sequence READ sequence NOTIFY sequenceChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)

    // Not part of the API, but this is the signed download URL.
    Q_PROPERTY(QString signedDownloadUrl READ signedDownloadUrl NOTIFY signedDownloadUrlChanged)

    // Not part of the API, but kept.
    Q_PROPERTY(QString clickToken READ clickToken NOTIFY clickTokenChanged)

    // Errors relating to parsing, fetching click tokens etc.
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

    const QString anonDownloadUrl();
    const uint binaryFilesize();
    const QString changelog();
    const QString channel();
    const QString content();
    const QString department();
    const QString downloadSha512();
    const QString downloadUrl();
    const QString iconUrl();
    const QString name();
    const QString origin();
    const QString packageName();
    const int revision();
    const int sequence();
    const QString status();
    const QString title();
    const QString version();

    const QString signedDownloadUrl();
    const QString clickToken();
    const QString errorString();

    // Abort any networking activity.
    void abort();

private slots:
    void tokenRequestSslFailed(const QList<QSslError> & errors);
    void tokenRequestFailed(const QNetworkReply::NetworkError code);
    void tokenRequestSucceeded(const QNetworkReply* reply);

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
    void versionChanged();

    void signedDownloadUrlChanged();
    void clickTokenChanged();

    void errorStringChanged();
    void parsingFailed();
    void signingFailed();

private:
    void setAnonDownloadUrl(const type &anonDownloadUrl);
    void setBinaryFilesize(const uint &binaryFilesize);
    void setChangelog(const QString &changelog);
    void setChannel(const QString &channel);
    void setContent(const QString &content);
    void setDepartment(const QString &department);
    void setDownloadSha512(const QString &downloadSha512);
    void setDownloadUrl(const QString &downloadUrl);
    void setDownloadUrl(const QString &downloadUrl);
    void setDownloadUrl(const QString &downloadUrl);
    void setIconUrl(const QString &iconUrl);
    void setName(const QString &name);
    void setOrigin(const QString &origin);
    void setPackageName(const QString &packageName);
    void setRevision(const QString &revision);
    void setSequence(const QString &sequence);
    void setStatus(const QString &status);
    void setTitle(const QString &title);
    void setVersion(const QString &version);

    void setSignedDownloadUrl(const QString &signedDownloadUrl);
    void setClickToken(const QString &clickToken);

    void parseMetadata();

    QJsonDocument m_metadata;
    UbuntuOne::Token m_token;
};

}

#endif // CLICKMETA_H
