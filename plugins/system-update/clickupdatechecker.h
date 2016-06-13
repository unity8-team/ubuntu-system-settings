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

#ifndef PLUGINS_SYSTEM_UPDATE_CLICKUPDATECHECKER_H_
#define PLUGINS_SYSTEM_UPDATE_CLICKUPDATECHECKER_H_

#include <QFile>
#include <QHash>
#include <QJsonDocument>
#include <QList>
#include <QProcess>

#include "clickupdatemetadata.h"

namespace UpdatePlugin
{
//
// Checks for click updates.
//
class ClickUpdateChecker : public ClickApiClient
{
Q_OBJECT
public:
    explicit ClickUpdateChecker(QObject *parent = 0);
    ~ClickUpdateChecker();

    // Check for click updates.
    void check();

    // Check for updates for this one package. Will try to refresh the
    // click token.
    void check(const QString &packageName);

    // Cancel check for click updates.
    void cancel();

protected slots:
    void requestSucceeded(QNetworkReply *reply);

private slots:
    void processInstalledClicks(const int &exitCode);
    void processClickToken(const ClickUpdateMetadata *meta);
    void handleProcessError(const QProcess::ProcessError &error);
    void handleClickTokenFailure(const ClickUpdateMetadata *meta);

signals:
    // Indicate that this ClickUpdateMetadata is pending.
    void updateAvailable(const ClickUpdateMetadata *meta);

    void checkStarted();
    void checkCompleted();
    void checkCanceled();
    void checkFailed();

private:
    // Set up connections on a ClickUpdateMetadata instance.
    void initializeMeta(const ClickUpdateMetadata *meta);

    // Set up connections on a process instance.
    void initializeProcess();

    // Start process of adding remote metadata to each installed click
    void requestClickMetadata();

    // Parses click metadata.
    // Note: This also asks a ClickUpdateMetadata to request a click token.
    // TODO: Make this more obvious.
    void parseClickMetadata(const QJsonArray &array);

    // Assert completion of check, signalling if check complete.
    void completionCheck();

    // Represents the process that we use to query installed clicks.
    QProcess m_process;

    // For each check we store metas until the next check.
    // We calculate whether or not we're done checking based
    // on whether or not there is a click token on a meta,
    // or if it has been removed from this list.
    QHash<QString, ClickUpdateMetadata*> m_metas;
};

}

#endif // PLUGINS_SYSTEM_UPDATE_CLICKUPDATECHECKER_H_
