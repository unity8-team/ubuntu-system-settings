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

#ifndef MOCK_IMAGE_MANAGER_H
#define MOCK_IMAGE_MANAGER_H

#include "image/imagemanager.h"

namespace UpdatePlugin
{
class MockImageManager : public Image::Manager
{
public:
    MockImageManager(QObject *parent = nullptr)
        : Image::Manager(parent) {};
    virtual ~MockImageManager() {};

    virtual void check() override
    {
        m_checkingForUpdates = true;
        Q_EMIT checkingForUpdatesChanged();
    }

    virtual void cancel() override
    {
        m_checkingForUpdates = false;
        Q_EMIT checkingForUpdatesChanged();
    }

    virtual bool checkingForUpdates() const override
    {
        return m_checkingForUpdates;
    }

    void mockChecking(const bool checking) {
        m_checkingForUpdates = checking;
        Q_EMIT checkingForUpdatesChanged();
    }

    bool m_checkingForUpdates = false;
    QString m_launched = QString::null;
};
} // UpdatePlugin
#endif // MOCK_IMAGE_MANAGER_H
