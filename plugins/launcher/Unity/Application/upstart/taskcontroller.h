/*
 * Copyright (C) 2013-2016 Canonical, Ltd.
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
 *
 */

#ifndef QTMIR_UPSTART_TASK_CONTROLLER_H
#define QTMIR_UPSTART_TASK_CONTROLLER_H

#include "../taskcontroller.h"

namespace qtmir
{
namespace upstart
{

class TaskController : public qtmir::TaskController
{
public:
    TaskController();
    ~TaskController();

    bool appIdHasProcessId(const QString& appId, pid_t pid) override;

    bool stop(const QString& appId) override;
    bool start(const QString& appId, const QStringList& arguments) override;

    bool suspend(const QString& appId) override;
    bool resume(const QString& appId) override;

    QSharedPointer<qtmir::ApplicationInfo> getInfoForApp(const QString &appId) const override;

private:
    struct Private;
    QScopedPointer<Private> impl;
};

} // namespace upstart
} // namespace qtmir

#endif // QTMIR_UPSTART_TASK_CONTROLLER_H
