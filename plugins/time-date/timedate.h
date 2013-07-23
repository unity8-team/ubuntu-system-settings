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

#include <QDBusInterface>
#include <QObject>
#include <QProcess>

class TimeDate : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString timeZone
                READ timeZone
                WRITE setTimeZone
                NOTIFY timeZoneChanged )
    Q_PROPERTY(QStringList allCities
               READ getCities
               NOTIFY citiesChanged)

public:
    explicit TimeDate(QObject *parent = 0);
    ~TimeDate();
    void setTimeZone (QString &time_zone);
    QString timeZone();
    const QStringList getCities();
    Q_INVOKABLE const QString getTimeZoneForCity(const QString city);

public Q_SLOTS:
    void slotChanged(QString, QVariantMap, QStringList);

Q_SIGNALS:
    void timeZoneChanged();
    void citiesChanged();

private:
    QString m_currentTimeZone;
    QDBusConnection m_systemBusConnection;
    QDBusInterface m_timeDateInterface;
    QMap<QString, QString> m_cityMap;
    QString m_objectPath;
    QString getTimeZone();
    QMap<QString, QString> buildCityMap();

};

#endif // TIMEDATE_H
