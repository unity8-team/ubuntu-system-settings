/*
 * Copyright (C) 2016 Canonical, Ltd.
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

#ifndef QTMIR_CGMANAGER_H
#define QTMIR_CGMANAGER_H

#include <QDBusConnection>

namespace qtmir {

class CGManager : public QObject {
    Q_OBJECT
public:
    CGManager(QObject *parent = nullptr);
    virtual ~CGManager();

    QString getCGroupOfPid(const QString &controller, pid_t pid);

    QSet<pid_t> getTasks(const QString &controller, const QString &cgroup);

private:
    const QString m_interface{"org.linuxcontainers.cgmanager0_0"};
    const QString m_path{"/org/linuxcontainers/cgmanager"};
    QDBusConnection getConnection();
};

} // namespace qtmir

#endif // QTMIR_CGMANAGER_H
