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

#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H

#include <QObject>
#include <QDebug>
#include <QString>

namespace UpdatePlugin
{
namespace Image
{
/* Image manager communicates and responds to system-image-dbus activity.
 *
 * See https://wiki.ubuntu.com/ImageBasedUpgrades/Client#DBus_API
 */
class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~Manager() {};

    // Check for updates.
    virtual void check() = 0;

    // Cancel a check for updates.
    virtual void cancel() = 0;

    // Return whether or not this manager is currently checking for updates.
    virtual bool checkingForUpdates() const = 0;
Q_SIGNALS:
    // This signal is emitted when check status changes.
    void checkingForUpdatesChanged();

    // This signal is emitted when a check completes.
    void checkCompleted();
};
} // Image
} // UpdatePlugin

#endif // IMAGE_MANAGER_H
