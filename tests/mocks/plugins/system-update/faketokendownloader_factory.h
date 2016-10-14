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
#include "click/tokendownloader.h"
#include "click/tokendownloader_factory.h"
#include "network/accessmanager.h"

#include "faketokendownloader.h"
#include "fakeapiclient.h"

using namespace UpdatePlugin;

class MockTokenDownloaderFactory
    : public Click::TokenDownloaderFactory
{
public:
    virtual Click::TokenDownloader* create(Network::Manager *nam,
                                           QSharedPointer<Update> update
    ) override
    {
        Q_UNUSED(nam);
        Click::ApiClient *client = new MockApiClient();
        MockTokenDownloader *d = new MockTokenDownloader(client, update);
        client->setParent(d);
        created.append(d);
        return d;
    }

    QList<MockTokenDownloader* > created;
};

#endif // FAKE_TOKEN_DOWNLOADERFACTORY_H
