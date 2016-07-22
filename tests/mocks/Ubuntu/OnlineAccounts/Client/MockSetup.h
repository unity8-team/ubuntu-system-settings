/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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
#ifndef MOCK_ONLINEACCOUNTS_CLIENT_SETUP_H
#define MOCK_ONLINEACCOUNTS_CLIENT_SETUP_H

#include <QObject>
#include <QString>
#include <QVariantMap>

namespace OnlineAccountsClient {

class MockSetup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString applicationId READ applicationId
               WRITE setApplicationId NOTIFY applicationIdChanged)
    Q_PROPERTY(QString providerId READ providerId
               WRITE setProviderId NOTIFY providerIdChanged)
public:
    MockSetup(QObject *parent = 0) : QObject(parent) {};
    ~MockSetup() {};

    void setApplicationId(const QString &applicationId);
    QString applicationId() const;

    void setProviderId(const QString &providerId);
    QString providerId() const;

    Q_INVOKABLE void exec();
    Q_INVOKABLE bool execCalled();
    Q_INVOKABLE void mockFinished(const QVariantMap &reply); // Mock only

Q_SIGNALS:
    void applicationIdChanged();
    void providerIdChanged();
    void finished(QVariantMap reply);

private:
    QString m_applicationId = QString::null;
    QString m_providerId = QString::null;
    bool m_execCalled = false;
};
}
#endif // MOCK_ONLINEACCOUNTS_CLIENT_SETUP_H
