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

#ifndef FAKE_CLICK_MANIFEST_H
#define FAKE_CLICK_MANIFEST_H

#include "click/manifest.h"

namespace UpdatePlugin
{
class MockManifest : public Click::Manifest
{
public:
    explicit MockManifest(QObject *parent = nullptr)
        : Click::Manifest(parent)
    {
    }

    virtual void request() override
    {
        asked = true;
    }

    void mockSuccess(const QJsonArray &manifest)
    {
        Q_EMIT requestSucceeded(manifest);
    }

    void mockFailure()
    {
        Q_EMIT requestFailed();
    }

    bool asked = false;
};
} // UpdatePlugin
#endif // FAKE_CLICK_MANIFEST_H
