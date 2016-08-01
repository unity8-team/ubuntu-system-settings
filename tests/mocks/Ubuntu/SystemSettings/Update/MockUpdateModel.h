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

#ifndef MOCK_UPDATE_MODEL_H
#define MOCK_UPDATE_MODEL_H

#include "updatemodel.h"
#include <QObject>
#include <QString>
#include <QDebug>

class MockUpdateModel : public UpdatePlugin::UpdateModel
{
    Q_OBJECT
public:
    MockUpdateModel(QObject *parent = nullptr)
        : UpdatePlugin::UpdateModel(":memory:", parent) {}

    Q_INVOKABLE void mockAddUpdate(const QString &id, const uint &revision,
                                   const uint &kind);
    Q_INVOKABLE void reset();
};

class MockUpdateModelFilter: public UpdatePlugin::UpdateModelFilter
{
    Q_OBJECT
public:
    explicit MockUpdateModelFilter(QObject *parent = nullptr)
        : UpdatePlugin::UpdateModelFilter(parent) {}

    Q_INVOKABLE void mockSetSourceModel(MockUpdateModel *source);
};
#endif // MOCK_UPDATE_MODEL_H
