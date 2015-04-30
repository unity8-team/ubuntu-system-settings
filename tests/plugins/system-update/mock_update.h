/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Manuel de la Pena <manuel.delapena@canonical.com>
 *
 */

#ifndef MOCKED_UPDATE_H
#define MOCKED_UPDATE_H

#include <gmock/gmock.h>

#include "update.h"

using namespace UpdatePlugin;

class MockedUpdate : public Update {
    Q_OBJECT
public:
    explicit MockedUpdate(QObject *parent = 0)
        : Update(parent) {}

    MOCK_METHOD1(setRemoteVersion, void(QString&));
    MOCK_METHOD1(setBinaryFilesize, void(int));
    MOCK_METHOD0(updateRequired, bool());
    MOCK_METHOD1(setIconUrl, void(QString));
    MOCK_METHOD1(setDownloadUrl, void(const QString&));
};

#endif
