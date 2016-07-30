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

#ifndef MOCK_SYSTEMIMAGE_H
#define MOCK_SYSTEMIMAGE_H

#include <QObject>
#include <QVariantMap>
#include <QDateTime>

class MockSystemImage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool checkingForUpdates READ checkingForUpdates
               NOTIFY checkingForUpdatesChanged)
    Q_PROPERTY(int currentBuildNumber READ currentBuildNumber
               NOTIFY currentBuildNumberChanged)
    Q_PROPERTY(int targetBuildNumber READ targetBuildNumber
               NOTIFY targetBuildNumberChanged)
public:
    explicit MockSystemImage(QObject *parent = nullptr)
        : QObject(parent) {};
    ~MockSystemImage() {};

    bool checkingForUpdates() const;
    int downloadMode();
    void setDownloadMode(const int &downloadMode);

    int currentBuildNumber() const;
    int targetBuildNumber() const;

    Q_INVOKABLE void checkForUpdate();
    Q_INVOKABLE void downloadUpdate();
    Q_INVOKABLE void forceAllowGSMDownload();
    Q_INVOKABLE void applyUpdate();
    Q_INVOKABLE QString cancelUpdate();
    Q_INVOKABLE QString pauseDownload();
    Q_INVOKABLE bool checkTarget() const;

    Q_INVOKABLE void mockTargetBuildNumber(const uint &target); // mock only
    Q_INVOKABLE void mockCurrentBuildNumber(const uint &current); // mock only
    Q_INVOKABLE bool called(const QString &functionName); // mock only
    Q_INVOKABLE void reset(); // mock only

Q_SIGNALS:
    void updateDownloaded();
    void checkingForUpdatesChanged();
    void currentBuildNumberChanged();
    void targetBuildNumberChanged();
private:
    bool m_checkingForUpdates = false;
    int m_currentBuildNumber = 0;
    int m_targetBuildNumber = -1;

    QStringList m_called;
};

#endif // MOCK_SYSTEMIMAGE_H
