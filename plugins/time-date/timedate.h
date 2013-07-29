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
 * Iain Lane <iain.lane@canonical.com>
 *
*/

#ifndef TIMEDATE_H
#define TIMEDATE_H

#include "timezonelocationmodel.h"
#include <QAbstractTableModel>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QObject>
#include <QProcess>

class TimeDate : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString timeZone
                READ timeZone
                WRITE setTimeZone
                NOTIFY timeZoneChanged)
    Q_PROPERTY (QAbstractItemModel *timeZoneModel
                READ getTimeZoneModel
                NOTIFY timeZoneModelChanged)
    Q_PROPERTY (QString filter
                READ getFilter
                WRITE setFilter)

public:
    explicit TimeDate(QObject *parent = 0);
    ~TimeDate();
    void setTimeZone (QString &time_zone);
    QString timeZone();
    QAbstractItemModel *getTimeZoneModel();
    QString getFilter();
    void setFilter (QString &filter);

public Q_SLOTS:
    void slotChanged(QString, QVariantMap, QStringList);
    void slotNameOwnerChanged(QString, QString, QString);

Q_SIGNALS:
    void timeZoneChanged();
    void timeZoneModelChanged();

private:
    QString m_currentTimeZone;
    QDBusConnection m_systemBusConnection;
    QDBusServiceWatcher m_serviceWatcher;
    QDBusInterface m_timeDateInterface;
    QString m_objectPath;
    TimeZoneLocationModel m_timeZoneModel;
    TimeZoneFilterProxy m_timeZoneFilterProxy;
    QString m_filter;
    QString getTimeZone();
    void setUpInterface();
    bool m_sortedBefore;

};

#endif // TIMEDATE_H
