/*
 * Copyright 2014 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef FAKESYSTEMUPDATE_H
#define FAKESYSTEMUPDATE_H

#include <QObject>
#include "../../../plugins/system-update/update.h"

namespace UpdatePlugin {

class FakeSystemUpdate: public QObject
{
    Q_OBJECT
public:
    explicit FakeSystemUpdate(QObject *parent = 0);
    ~FakeSystemUpdate() {}

    int downloadMode() { return 0; }
    void setDownloadMode(int) {}
    int currentBuildNumber() { return 123;}

    void checkForUpdate() {}
    void downloadUpdate() {}
    void applyUpdate() {}
    void cancelUpdate() {}
    void pauseDownload() {}

Q_SIGNALS:
    void updateAvailable(const QString& packageName, Update *update);
    void updateNotFound();
    void updateProgress(int percentage, double eta);
    void updatePaused(int percentage);
    void updateDownloaded();
    void updateFailed(int consecutiveFailureCount, QString lastReason);
    void downloadModeChanged();
    void updateProcessFailed(const QString& reason);
    
};

}

#endif // FAKESYSTEMUPDATE_H
