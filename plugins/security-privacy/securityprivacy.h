/*
 * Copyright (C) 2012,2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Michael Terry <michael.terry@canonical.com>
 *          Iain Lane <iain.lane@canonical.com>
 */

#ifndef SECURITYPRIVACY_H
#define SECURITYPRIVACY_H

#include <QDBusServiceWatcher>
#include <QStringList>
#include <QtDBus/QDBusInterface>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>

class SecurityPrivacy: public QObject
{
    Q_OBJECT
    Q_PROPERTY (bool statsWelcomeScreen
                READ getStatsWelcomeScreen
                WRITE setStatsWelcomeScreen
                NOTIFY statsWelcomeScreenChanged)
    Q_PROPERTY (bool messagesWelcomeScreen
                READ getMessagesWelcomeScreen
                WRITE setMessagesWelcomeScreen
                NOTIFY messagesWelcomeScreenChanged)

public:
    explicit SecurityPrivacy(QObject *parent = 0);
    bool getStatsWelcomeScreen();
    void setStatsWelcomeScreen(bool enabled);
    bool getMessagesWelcomeScreen();
    void setMessagesWelcomeScreen(bool enabled);

public Q_SLOTS:
    void slotChanged(QString, QVariantMap, QStringList);
    void slotNameOwnerChanged(QString, QString, QString);

Q_SIGNALS:
    void statsWelcomeScreenChanged();
    void messagesWelcomeScreenChanged();

private:
    QDBusConnection m_systemBusConnection;
    QDBusServiceWatcher m_serviceWatcher;
    QDBusInterface m_accountsserviceIface;
    QString m_objectPath;

    QVariant getUserProperty(const QString &property);
    void setUserProperty(const QString &property,
                         const QVariant &value);
    void setUpInterface();

};

#endif //SECURITYPRIVACY_H
