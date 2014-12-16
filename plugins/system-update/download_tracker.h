/*
 * Copyright (C) 2014 - Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Diego Sarmentero <diego.sarmentero@canonical.com>
 */


#ifndef DOWNLOADTRACKER_H
#define DOWNLOADTRACKER_H

#include <QObject>
#include <QtQml>
#include <QList>
#include <QString>
#include <ubuntu/download_manager/download.h>
#include <ubuntu/download_manager/manager.h>

using Ubuntu::DownloadManager::Download;
using Ubuntu::DownloadManager::Manager;

namespace Ubuntu { namespace DownloadManager { class Error; } }

namespace UpdatePlugin {

class DownloadTracker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString clickToken READ clickToken WRITE setClickToken)
    Q_PROPERTY(QString download READ download WRITE setDownload)
    Q_PROPERTY(QString packageName READ packageName WRITE setPackageName)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)

public:
    explicit DownloadTracker(QObject *parent = 0);
    ~DownloadTracker() {}

    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();

    QString download() { return m_downloadUrl; }
    QString clickToken() { return m_clickToken; }
    QString packageName() { return m_packageName; }
    void setDownload(const QString& url);
    void setClickToken(const QString& token);
    void setPackageName(const QString& package);
    int progress() { return m_progress; }

public Q_SLOTS:
    void bindDownload(Download* download);
    void setProgress(qulonglong received, qulonglong total);
    void registerError(Ubuntu::DownloadManager::Error* error);
    void onDownloadFinished(const QString& path);
    void onDownloadCanceled(bool wasCanceled);

Q_SIGNALS:
    void error(const QString &errorMessage);
    void finished(const QString &path);
    void started(bool success);
    void canceled(bool success);
    void paused(bool success);
    void resumed(bool success);
    void processing(const QString &path);
    void progressChanged();
    void errorFound(const QString &error);

private:
    QString m_clickToken = QString::null;
    QString m_downloadUrl = QString::null;
    QString m_packageName = QString::null;
    Download* m_download = nullptr;
    Manager* m_manager = nullptr;
    int m_progress = 0;

    void startService();
    QString getPkconCommand();
};

}

#endif // DOWNLOADTRACKER_H
