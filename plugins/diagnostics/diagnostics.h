/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Evan Dandrea <evan.dandrea@canonical.com>
 *
*/

#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <QDBusInterface>
#include <QObject>
#include <QProcess>

class Diagnostics : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool canReportCrashes
                READ canReportCrashes
                NOTIFY reportCrashesChanged )
    
public:
    explicit Diagnostics(QObject *parent = 0);
    ~Diagnostics();
    bool canReportCrashes();
    Q_INVOKABLE void setReportCrashes(bool report);
    Q_INVOKABLE QString getIdentifier();

public Q_SLOTS:
    void slotChanged();

Q_SIGNALS:
    void reportCrashesChanged();

private:
    QDBusConnection m_systemBusConnection;
};

#endif // DIAGNOSTICS_H
