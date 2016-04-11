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
*/
#ifndef CLICKUPDATECHECKER_H
#define CLICKUPDATECHECKER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace UpdatePlugin {

class ClickUpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit ClickUpdateChecker(QObject *parent = 0);
    ~ClickUpdateChecker() {}

    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

    void checkForUpdates();
    void abortCheckForUpdates();
    void setToken(const UbuntuOne::Token &token);

    QString errorString() const;

private slots:
    void requestSucceded();
    void requestFailed();
    void requestSslFailed();
    void processedInstalledClicks(const int &exitCode);

signals:
    void errorStringChanged();
    void foundClickUpdate(const ClickUpdateMetadata &clickUpdateMetadata);

    // Check completed, i.e. all installed clicks have been checked.
    void checkCompleted();

    // The check failed, but this does not include failures for individual
    // click updates; those failures will merely be logged and ignored.
    void checkFailed();

private:
    QProcess m_process;
    UbuntuOne::Token m_token;
    QList<ClickUpdateMetadata> m_clickUpdateMetadatas;
};

}

#endif // CLICKUPDATECHECKER_H
