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

#include <QList>
#include <QSet>

#include "application.h"

namespace qtmir {

class CGManager;
class MirSurfaceInterface;

/*
   Enables other processes to check what is the currently focused application or surface,
   normally for security purposes.
 */
class DBusFocusInfo : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Unity.FocusInfo")
public:
    explicit DBusFocusInfo(const QList<Application*> &applications);
    virtual ~DBusFocusInfo() {}

public Q_SLOTS:

    /*
        Returns true if the application with the given PID has input focus

        FIXME: Identifying an app through its PID is deemed racy.
               isSurfaceFocused() is the preferred method.
     */
    Q_SCRIPTABLE bool isPidFocused(unsigned int pid);

    /*
        Returns true if the surface with the given id has input focus
     */
    Q_SCRIPTABLE bool isSurfaceFocused(const QString &surfaceId);

private:
    QSet<pid_t> fetchAssociatedPids(pid_t pid);
    SessionInterface* findSessionWithPid(const QSet<pid_t> &pidSet);
    SessionInterface* findSessionWithPid(SessionInterface* session, const QSet<pid_t> &pidSet);
    MirSurfaceInterface *findQmlSurface(const QString &serializedId);

    const QList<Application*> &m_applications;

    CGManager *m_cgManager;
};

} // namespace qtmir
