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

#include <QHash>
#include <QProcess>
#include <QSharedPointer>

#include "clickupdatemetadata.h"

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

    void check();
    void cancel();

protected slots:
    void requestSucceeded(QNetworkReply *reply);

private slots:
    void handleInstalledClicks(const int &exitCode);
    void handleProcessError(const QProcess::ProcessError &error);
    void handleMetadataClickTokenObtained(const ClickUpdateMetadata *meta);
    void handleClickTokenRequestFailed(const ClickUpdateMetadata *meta);

signals:
    void updateAvailable(
        const QSharedPointer<ClickUpdateMetadata> &meta);
    void checkCompleted();

private:
    void initializeMeta(const QSharedPointer<ClickUpdateMetadata> &meta);
    void initializeProcess();

    // Starts process of adding remote metadata to each installed click
    void requestClickMetadata();

    QProcess m_process;
    QHash<QString, QSharedPointer<ClickUpdateMetadata> > m_metas;
};

}

#endif // CLICKUPDATECHECKER_H
