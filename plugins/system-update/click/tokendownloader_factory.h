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

#ifndef CLICK_TOKEN_DOWNLOADER_FACTORY_H
#define CLICK_TOKEN_DOWNLOADER_FACTORY_H

#include "click/tokendownloader.h"
#include "network/accessmanager.h"

#include <QObject>

namespace UpdatePlugin
{
namespace Click
{
class TokenDownloaderFactory : public QObject
{
    Q_OBJECT
public:
    virtual ~TokenDownloaderFactory() {};
    // Creates a TokenDownloader.
    virtual TokenDownloader* create(Network::Manager *nam,
                                    QSharedPointer<Update> update,
                                    QObject *parent = nullptr) = 0;
};
} // Click
} // UpdatePlugin

#endif // CLICK_TOKEN_DOWNLOADER_FACTORY_H