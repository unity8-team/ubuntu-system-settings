/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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

#ifndef ACCOUNTSSERVICE_H
#define ACCOUNTSSERVICE_H

#include <QDBusServiceWatcher>
#include <QStringList>
#include <QtDBus/QDBusInterface>

class AccountsService : public QObject
{
    Q_OBJECT

public:
    explicit AccountsService (QObject *parent = 0);

    QString getProperty (QString property);
    QVariant getUserProperty(const QString &interface,
                             const QString &property);
    bool setUserProperty(const QString &interface,
                         const QString &property,
                         const QVariant &value);
    bool customSetUserProperty(const QString &method,
                               const QVariant &value);


public Q_SLOTS:
    void slotChanged(QString, QVariantMap, QStringList);
    void slotNameOwnerChanged(QString, QString, QString);

Q_SIGNALS:
    void propertyChanged(QString interface, QString property);
    void changed();
    void nameOwnerChanged();

private:
    QDBusConnection m_systemBusConnection;
    QDBusServiceWatcher m_serviceWatcher;
    QDBusInterface m_accountsserviceIface;
    QString m_objectPath;

    void setUpInterface();

};

#endif // ACCOUNTSSERVICE_H
