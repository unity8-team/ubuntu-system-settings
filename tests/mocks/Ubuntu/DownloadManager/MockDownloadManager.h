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

#ifndef MOCK_DOWNLOADMANAGER_H
#define MOCK_DOWNLOADMANAGER_H

#include "MockSingleDownload.h"

#include <QObject>
#include <QVariant>
#include <QVariantList>

class MockDownloadManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(QVariantList downloads READ downloads NOTIFY downloadsChanged)

public:
    explicit MockDownloadManager(QObject *parent = 0);
    ~MockDownloadManager();

    Q_INVOKABLE void download(QString url);

    QVariantList downloads();
    QString errorMessage() const;
    bool autoStart() const;
    void setAutoStart(bool value);

    Q_INVOKABLE void mockDownload(MockSingleDownload *download); // mock only
    Q_INVOKABLE void mockDownloadFinished(MockSingleDownload *download,
                                          const QString &path); // mock only
    Q_INVOKABLE void mockDownloadPaused(MockSingleDownload *download); // mock only
    Q_INVOKABLE void mockDownloadResumed(MockSingleDownload *download); // mock only
    Q_INVOKABLE void mockDownloadCanceled(MockSingleDownload *download); // mock only
    Q_INVOKABLE void mockErrorFound(MockSingleDownload *download,
                                    const QString &error); // mock only

signals:
    void errorChanged();
    void downloadsChanged();
    void downloadFinished(MockSingleDownload *download, const QString& path);
    void errorFound(MockSingleDownload *download);
    void downloadPaused(MockSingleDownload *download);
    void downloadResumed(MockSingleDownload *download);
    void downloadCanceled(MockSingleDownload *download);

private:
    QVariantList m_downloads;
};

#endif // MOCK_DOWNLOADMANAGER_H
