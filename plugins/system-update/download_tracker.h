/*
 * Copyright (C) 2013 - Canonical Ltd.
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
#include <ubuntu/download_manager/download.h>
//#include <ubuntu/download_manager/error.h>
#include <ubuntu/download_manager/manager.h>

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
    ~DownloadTracker();

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
    void bindDownload(Ubuntu::DownloadManager::Download* download);
    void setProgress(qulonglong received, qulonglong total);
//    void errorReceived(Ubuntu::DownloadManager::Error* e);

Q_SIGNALS:
    void error(const QString &errorMessage);
    void finished(const QString &path);
    void progressChanged();

private:
    QString m_clickToken;
    QString m_downloadUrl;
    QString m_packageName;
    Ubuntu::DownloadManager::Download* m_download;
    Ubuntu::DownloadManager::Manager* m_manager;
    int m_progress;

    void startService();
    QString getPkconCommand();
};

}

#endif // DOWNLOADTRACKER_H
