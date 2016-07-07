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


#ifndef FAKE_TOKEN_DOWNLOADERFACTORY_H
#define FAKE_TOKEN_DOWNLOADERFACTORY_H

#include "update.h"
#include "client.h"
#include "tokendownloader.h"
#include "tokendownloader_factory.h"

#include "faketokendownloader.h"

class MockTokenDownloaderFactory
    : public UpdatePlugin::Click::TokenDownloaderFactory
{
public:
    virtual UpdatePlugin::Click::TokenDownloader* create(
        UpdatePlugin::Click::Client *client,
        QSharedPointer<UpdatePlugin::Update> update,
        QObject *parent = 0
    ) override
    {
        Q_UNUSED(client)
        MockTokenDownloader *d = new MockTokenDownloader(update, parent);
        created.append(d);
        return d;
    }

    QList<MockTokenDownloader* > created;
};

#endif // FAKE_TOKEN_DOWNLOADERFACTORY_H
