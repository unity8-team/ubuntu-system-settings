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

#include "accountsservice.h"

#include <QDBusServiceWatcher>
#include <QStringList>
#include <QtDBus/QDBusInterface>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QString>

typedef struct _ActUser ActUser;
typedef struct _ActUserManager ActUserManager;
typedef struct _GObject GObject;
typedef struct _GParamSpec GParamSpec;
typedef void *gpointer;

class SecurityPrivacy: public QObject
{
    Q_OBJECT
    Q_ENUMS(SecurityType)
    Q_PROPERTY (bool statsWelcomeScreen
                READ getStatsWelcomeScreen
                WRITE setStatsWelcomeScreen
                NOTIFY statsWelcomeScreenChanged)
    Q_PROPERTY (bool messagesWelcomeScreen
                READ getMessagesWelcomeScreen
                WRITE setMessagesWelcomeScreen
                NOTIFY messagesWelcomeScreenChanged)
    Q_PROPERTY (bool enableLauncherWhileLocked
                READ getEnableLauncherWhileLocked
                WRITE setEnableLauncherWhileLocked
                NOTIFY enableLauncherWhileLockedChanged)
    Q_PROPERTY (bool enableIndicatorsWhileLocked
                READ getEnableIndicatorsWhileLocked
                WRITE setEnableIndicatorsWhileLocked
                NOTIFY enableIndicatorsWhileLockedChanged)
    Q_PROPERTY (SecurityType securityType
                READ getSecurityType
                NOTIFY securityTypeChanged)

public:
    enum SecurityType {
         Swipe,
         Passcode,
         Passphrase
    };

    explicit SecurityPrivacy(QObject *parent = 0);
    virtual ~SecurityPrivacy();

    bool getStatsWelcomeScreen();
    void setStatsWelcomeScreen(bool enabled);
    bool getMessagesWelcomeScreen();
    void setMessagesWelcomeScreen(bool enabled);
    bool getEnableLauncherWhileLocked();
    void setEnableLauncherWhileLocked(bool enabled);
    bool getEnableIndicatorsWhileLocked();
    void setEnableIndicatorsWhileLocked(bool enabled);
    SecurityType getSecurityType();

    // Returns error text, if an error occurred
    Q_INVOKABLE QString setSecurity(QString oldValue, QString value, SecurityType type);
    Q_INVOKABLE bool trySetSecurity(SecurityType type);

public Q_SLOTS:
    void slotChanged(QString, QString);
    void slotNameOwnerChanged();

Q_SIGNALS:
    void statsWelcomeScreenChanged();
    void messagesWelcomeScreenChanged();
    void enableLauncherWhileLockedChanged();
    void enableIndicatorsWhileLockedChanged();
    void securityTypeChanged();

private:
    void loadUser();

    void managerLoaded();
    friend void managerLoaded(GObject    *object,
                              GParamSpec *pspec,
                              gpointer    user_data);

    void userLoaded();
    friend void userLoaded(GObject    *object,
                           GParamSpec *pspec,
                           gpointer    user_data);

    QString badPasswordMessage(SecurityType type);
    bool setDisplayHint(SecurityType type);
    bool setPasswordMode(SecurityType type);
    bool setPasswordModeWithPolicykit(SecurityType type, QString password);
    QString setPassword(QString oldValue, QString value);

    AccountsService m_accountsService;
    ActUserManager *m_manager;
    ActUser *m_user;
    QString m_username;
};

#endif //SECURITYPRIVACY_H
