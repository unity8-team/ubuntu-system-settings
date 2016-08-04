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
 *
 */

#include "imagemanager_impl.h"
        #include <QDebug>
namespace UpdatePlugin
{
namespace Image
{
const QString ManagerImpl::ubuntuId = QString("ubuntu");
ManagerImpl::ManagerImpl(UpdateModel *model, QObject *parent)
    : ManagerImpl(new QSystemImage(), model, parent)
{
    m_si->setParent(this);
}

ManagerImpl::ManagerImpl(QSystemImage *si, UpdateModel *model, QObject *parent)
    : Manager(parent)
    , m_model(model)
    , m_si(si)
{
    connect(m_si, SIGNAL(checkingForUpdatesChanged()),
            this, SLOT(handleCheckingForUpdatesChanged()));
    connect(
        m_si,
        SIGNAL(updateAvailableStatus(const bool, const bool, const QString&,
                                     const int&, const QString&,
                                     const QString&)),
        this,
        SLOT(handleUpdateAvailableStatus(const bool, const bool,
                                         const QString&, const int&,
                                         const QString&, const QString&))
    );
    connect(m_si, SIGNAL(downloadStarted()),
            this, SLOT(handleDownloadStarted()));
    connect(m_si, SIGNAL(updateProgress(const int&, const double&)),
            this, SLOT(handleUpdateProgress(const int&, const double&)));
    connect(m_si, SIGNAL(updatePaused(const int&)),
            this, SLOT(handleUpdatePaused(const int&)));
    connect(m_si, SIGNAL(updateDownloaded()),
            this, SLOT(handleUpdateDownloaded()));
    connect(m_si, SIGNAL(updateFailed(const int&, const QString&)),
            this, SLOT(handleUpdateFailed(const int&, const QString&)));
    connect(m_si, SIGNAL(currentBuildNumberChanged()),
            this, SLOT(handleCurrentBuildNumberChanged()));
    connect(m_si, SIGNAL(rebooting(const bool)),
            this, SLOT(handleRebooting(const bool)));
    connect(m_si, SIGNAL(updateProcessing()),
            this, SLOT(handleUpdateProcessing()));
    connect(m_si, SIGNAL(updateProcessFailed(const QString&)),
            this, SLOT(handleUpdateProcessFailed(const QString&)));

    /* If we have a pending image update here that has started, make sure
    we call DownloadUpdate so as to capture the UpdateDownloaded event. */
    auto update = m_model->get(ubuntuId, m_si->targetBuildNumber());
    if (update && update->state() == Update::State::StateDownloading) {
        m_si->downloadUpdate();
    }

    /* This is currently the best we can do for marking image updates
    as, as signals aren't usually sent from s-i.installed. lp:1600449 */
    handleCurrentBuildNumberChanged();
}

void ManagerImpl::handleUpdateAvailableStatus(const bool isAvailable,
                                              const bool downloading,
                                              const QString &availableVersion,
                                              const int &updateSize,
                                              const QString &lastUpdateDate,
                                              const QString &errorReason)
{
    Q_UNUSED(lastUpdateDate)

    bool ok;
    int rev;
    rev = availableVersion.toInt(&ok);
    if (!ok) {
        qWarning() << Q_FUNC_INFO << "Got non-numerical version, ignoring.";
        return;
    }
    if (isAvailable) {
        m_model->setImageUpdate(ubuntuId, rev, updateSize);
        bool automatic = m_si->downloadMode() > 0;
        if (downloading) {
            m_model->startUpdate(ubuntuId, rev, automatic);
        } else if (errorReason == "paused") {
            m_model->pauseUpdate(ubuntuId, rev, automatic);
        }
    }

    Q_EMIT checkCompleted();
}

void ManagerImpl::handleDownloadStarted()
{
    m_model->setProgress(ubuntuId, m_si->targetBuildNumber(), 0);
}

void ManagerImpl::handleUpdateProgress(const int &percentage, const double &eta)
{
    Q_UNUSED(eta)
    m_model->setProgress(ubuntuId, m_si->targetBuildNumber(), percentage);
}

void ManagerImpl::handleUpdatePaused(const int &percentage)
{
    m_model->setProgress(ubuntuId, m_si->targetBuildNumber(), percentage);
    m_model->pauseUpdate(ubuntuId, m_si->targetBuildNumber());
}

void ManagerImpl::handleUpdateDownloaded()
{
    m_model->setDownloaded(ubuntuId, m_si->targetBuildNumber());
}

void ManagerImpl::handleUpdateFailed(const int &consecutiveFailureCount, const QString &lastReason)
{
    Q_UNUSED(consecutiveFailureCount)
    m_model->setError(ubuntuId, m_si->targetBuildNumber(), lastReason);
}

void ManagerImpl::handleCurrentBuildNumberChanged()
{
    /* This is currently the best we can do for marking image updates
    as installed, as signals aren't usually sent from s-i. lp:1600449 */
    m_model->setInstalled(ubuntuId, m_si->currentBuildNumber());
}

void ManagerImpl::handleRebooting(const bool status)
{
    if (status) {
        m_model->setInstalling(ubuntuId, m_si->targetBuildNumber(), 5);
    } else {
        m_model->setError(ubuntuId, m_si->targetBuildNumber(),
                          "Failed to restart device.");
    }
}

void ManagerImpl::handleUpdateProcessing()
{
    m_model->setInstalling(ubuntuId, m_si->targetBuildNumber());
}

void ManagerImpl::handleUpdateProcessFailed(const QString &reason)
{
    m_model->setError(ubuntuId, m_si->targetBuildNumber(), reason);
}

void ManagerImpl::handleCheckingForUpdatesChanged()
{
    Q_EMIT checkingForUpdatesChanged();
}

void ManagerImpl::check()
{
    m_si->checkForUpdate();
}

void ManagerImpl::cancel()
{
    QString ret = m_si->cancelUpdate();
    if (!ret.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "failed to cancel" << ret;
    }
}

bool ManagerImpl::checkingForUpdates() const
{
    return m_si->checkingForUpdates();
}
} // Image
} // UpdatePlugin
