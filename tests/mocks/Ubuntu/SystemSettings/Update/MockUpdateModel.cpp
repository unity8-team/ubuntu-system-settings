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
    QSharedPointer<Update> u = QSharedPointer<Update>(new Update);
    u->setIdentifier(id);
    u->setKind((Update::Kind) kind);
    u->setRevision(revision);
    u->setTitle(QString("Test App %1").arg(id));
    u->setRemoteVersion(QString("v%1").arg(revision));
    u->setBinaryFilesize(5000 * 1000);
    db()->add(u);
    refresh();
}
