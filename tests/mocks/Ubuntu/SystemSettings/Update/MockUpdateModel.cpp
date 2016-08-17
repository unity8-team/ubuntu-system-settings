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

#include "MockUpdateModel.h"

#include <QSharedPointer>

void MockUpdateModel::mockAddUpdate(const QString &id, const uint &revision, const uint &kind)
{
    using namespace UpdatePlugin;
    auto update = QSharedPointer<Update>(new Update);
    update->setIdentifier(id);
    update->setKind((Update::Kind) kind);
    update->setRevision(revision);
    update->setTitle(QString("Test App %1").arg(id));
    update->setRemoteVersion(QString("v%1").arg(revision));
    update->setBinaryFilesize(5000 * 1000);
    add(update);
}

void MockUpdateModel::reset()
{
    UpdateModel::reset();
}

void MockUpdateModelFilter::mockSetSourceModel(MockUpdateModel *source)
{
    setSourceModel(source);
}
