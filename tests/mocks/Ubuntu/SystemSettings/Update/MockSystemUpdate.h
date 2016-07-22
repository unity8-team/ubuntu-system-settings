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
#ifndef MOCK_SYSTEM_UPDATE_H
#define MOCK_SYSTEM_UPDATE_H

#include "systemupdate.h"
#include "MockUpdateModel.h"

using namespace UpdatePlugin;

class MockSystemUpdate : public SystemUpdate
{
    Q_OBJECT
public:
    static MockSystemUpdate *instance();

    Q_INVOKABLE bool isCheckRequired();
    Q_INVOKABLE void mockIsCheckRequired(const bool isRequired); // mock only

protected:
    explicit MockSystemUpdate(MockUpdateModel *model, QObject *parent = 0);
    ~MockSystemUpdate() {}
private:
    static MockSystemUpdate *m_instance;

    bool m_checkRequired = false;
};

#endif // MOCK_SYSTEM_UPDATE_H
