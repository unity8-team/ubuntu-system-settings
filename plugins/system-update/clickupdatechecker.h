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

#include <QFile>
#include <QHash>
#include <QJsonDocument>
#include <QList>
#include <QProcess>
#include <QSharedPointer>

#include "clickupdatemetadata.h"
#include "clickapicache.h"

namespace UpdatePlugin {

//
// Checks for click updates.
//
class ClickUpdateChecker : public ClickApiProto
{
    Q_OBJECT
public:
    explicit ClickUpdateChecker(QObject *parent = 0);
    ~ClickUpdateChecker();

    // Check for click updates.
    void check();

    // Cancel check for click updates.
    void cancel();

    // Return the amount of cached click updates.
    int cachedCount();

protected slots:
    void requestSucceeded(QNetworkReply *reply);

private slots:
    void processInstalledClicks(const int &exitCode);
    void processClickToken(const ClickUpdateMetadata *meta);
    void handleProcessError(const QProcess::ProcessError &error);
    void handleClickTokenFailure(const ClickUpdateMetadata *meta);

signals:
    // Indicate that this ClickUpdateMetadata is ready for download and
    // install.
    void updateAvailable(
        const QSharedPointer<ClickUpdateMetadata> &meta);

    // Indicate that the check has been completed.
    void checkCompleted();

private:
    // Set up connections on a ClickUpdateMetadata instance.
    void initializeMeta(const QSharedPointer<ClickUpdateMetadata> &meta);

    // Set up connections on a process instance (used for click list …)
    void initializeProcess();

    // Start process of adding remote metadata to each installed click
    void requestClickMetadata();

    // Parses click metadata.
    // Note: This also asks a ClickUpdateMetadata to request a click token.
    // TODO: Make this more obvious.
    void parseClickMetadata(const QJsonArray &array);

    QProcess m_process;
    QHash<QString, QSharedPointer<ClickUpdateMetadata> > m_metas;
    ClickApiCache m_apiCache;
};

}

#endif // CLICKUPDATECHECKER_H
