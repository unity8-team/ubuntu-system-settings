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

#ifndef IMAGE_MANAGER_IMPL_H
#define IMAGE_MANAGER_IMPL_H

#include "imagemanager.h"
#include "systemimage.h"
#include "updatemodel.h"

namespace UpdatePlugin
{
namespace Image
{
class ManagerImpl : public Manager
{
    Q_OBJECT
public:
    explicit ManagerImpl(UpdateModel *model, QObject *parent = nullptr);
    explicit ManagerImpl(QSystemImage *si,
                         UpdateModel *model,
                         QObject *parent = nullptr);
    ~ManagerImpl() {};
    virtual void check() override;
    virtual void cancel() override;
    virtual bool checkingForUpdates() const;
    static const QString ubuntuId;
private Q_SLOTS:
    void handleUpdateAvailableStatus(const bool isAvailable,
                                     const bool downloading,
                                     const QString &availableVersion,
                                     const int &updateSize,
                                     const QString &lastUpdateDate,
                                     const QString &errorReason);
    void handleDownloadStarted();
    void handleUpdateProgress(const int &percentage, const double &eta);
    void handleUpdatePaused(const int &percentage);
    void handleUpdateDownloaded();
    void handleUpdateFailed(const int &consecutiveFailureCount, const QString &lastReason);
    void handleCurrentBuildNumberChanged();
    void handleRebooting(const bool status);
    void handleUpdateProcessing();
    void handleUpdateProcessFailed(const QString &reason);
    void handleCheckingForUpdatesChanged();
private:
    UpdateModel *m_model;
    QSystemImage *m_si;
};
} // Image
} // UpdatePlugin

#endif // IMAGE_MANAGER_IMPL_H
