/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <QHash>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "update.h"

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin {

class RequestObject : public QObject
{
    Q_OBJECT
public:
    explicit RequestObject(QString oper, QObject *parent = 0) :
        QObject(parent)
    {
        operation = oper;
    }

    QString operation;
};

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);

    void checkForNewVersions(QHash<QString, Update*> &apps);
    void getClickToken(Update *app, const QString &url,
                       const QString &authHeader);
protected:
    // helper constructor that allows to set the hash map for testing purposes
    Network(QHash<QString, Update*> apps, QObject *parent=0);

Q_SIGNALS:
    void updatesFound();
    void updatesNotFound();
    void errorOccurred();
    void networkError();
    void serverError();
    void clickTokenObtained(Update *app, const QString &clickToken);

protected:
    QUrl getUrlApps();
    bool parseUpdateObject(const QJsonValue& value);

private Q_SLOTS:
    void onRequestFinished();
    void onHeadRequestFinished();

private:
    QNetworkAccessManager m_nam;
    QHash<QString, Update*> m_apps;
};

}

#endif // NETWORK_H
