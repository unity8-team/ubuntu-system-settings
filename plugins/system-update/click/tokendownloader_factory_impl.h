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

#ifndef CLICK_TOKEN_DOWNLOADER_FACTORY_IMPL_H
#define CLICK_TOKEN_DOWNLOADER_FACTORY_IMPL_H

#include "click/tokendownloader_factory.h"

namespace UpdatePlugin
{
namespace Click
{
class TokenDownloaderFactoryImpl : public TokenDownloaderFactory
{
public:
    virtual TokenDownloader* create(Network::Manager *nam,
                                    QSharedPointer<Update> update) override;
};
} // Click
} // UpdatePlugin

#endif // CLICK_TOKEN_DOWNLOADER_FACTORY_IMPL_H
