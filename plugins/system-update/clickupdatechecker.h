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

#include "clickapiproto.h"

namespace UpdatePlugin {

//
// Gathers metadata on locally installed clicks and signals to listeners
// if any clicks needs updating.
//
class ClickUpdateChecker : public ClickApiProto
{
    Q_OBJECT
public:
    explicit ClickUpdateChecker(QObject *parent = 0);
    ~ClickUpdateChecker();

    void checkForUpdates();
    void abortCheckForUpdates();

private slots:
    void handleInstalledClicks(const int &exitCode);
    void handleDownloadUrlSigned();
    void handleDownloadUrlSignFailure();

signals:
    void clickUpdateDownloadable(const ClickUpdateMetadata &meta);
    void checkCompleted();

private:
    void setUpMeta(const ClickUpdateMetadata &meta);
    void setUpProcess();

    // Starts process of adding remote metadata to each installed click
    void requestClickMetadata();

    QProcess m_process;
    QMap<QString, ClickUpdateMetadata> m_metas;
};

}

#endif // CLICKUPDATECHECKER_H
