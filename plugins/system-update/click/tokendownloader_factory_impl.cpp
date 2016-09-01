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

#include "click/apiclient_impl.h"
#include "click/tokendownloader_impl.h"
#include "click/tokendownloader_factory_impl.h"

namespace UpdatePlugin
{
namespace Click
{
TokenDownloader* TokenDownloaderFactoryImpl::create(Network::Manager *nam,
                                                    QSharedPointer<Update> update)
{
    auto client = new Click::ApiClientImpl(nam);
    auto downloader = new TokenDownloaderImpl(client, update);
    client->setParent(downloader);
    return downloader;
}
} // Click
} // UpdatePlugin
