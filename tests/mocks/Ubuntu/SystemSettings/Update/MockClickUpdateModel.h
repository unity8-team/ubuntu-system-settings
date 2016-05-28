/*
 * Copyright (C) 2016 Canonical Ltd
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
*/

#ifndef MOCK_CLICK_UPDATE_MODEL_H
#define MOCK_CLICK_UPDATE_MODEL_H

#include <QObject>

class MockClickUpdateModel : public QObject
{
    Q_OBJECT
public:
    MockClickUpdateModel(QObject *parent = 0)
    {
    }

    ~MockClickUpdateModel()
    {
    }
};

#endif // MOCK_CLICK_UPDATE_MODEL_H
