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

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QHash>
#include "../update.h"

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin {

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);

    void checkForNewVersions(QHash<QString, Update*> &apps);
    void getResourceUrl(const QString &packagename);
    void getClickToken(Update *app, const QString &url,
                       const QString &authHeader);

Q_SIGNALS:
    void updatesFound();
    void updatesNotFound();
    void errorOccurred();
    void downloadUrlFound(const QString &packagename, const QString &url);
    void clickTokenObtained(Update *app, const QString &clickToken);

private Q_SLOTS:
    void onReply(QNetworkReply*);

private:
    QNetworkAccessManager m_nam;
    QNetworkRequest m_request;
    QHash<QString, Update*> m_apps;

    QString getUrlApps();
    QString getUrlPackage();
};

}

#endif // NETWORK_H
