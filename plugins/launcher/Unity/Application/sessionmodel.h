/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H

// Local
#include "objectlistmodel.h"

namespace qtmir {

class SessionInterface;
typedef ObjectListModel<SessionInterface> SessionModel;

} // namespace qtmir

Q_DECLARE_METATYPE(qtmir::SessionModel*)

#endif // SESSIONMODEL_H
