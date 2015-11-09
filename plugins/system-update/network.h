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

#include <token.h>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkReply>
#include <QHash>

#include "update.h"

#define X_CLICK_TOKEN "X-Click-Token"

namespace UpdatePlugin {

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);

    void checkForNewVersions(QHash<QString, Update*> &apps);
    void getClickToken(Update *app, const QString &url);

    virtual std::vector<std::string> getAvailableFrameworks();
    virtual std::string getArchitecture();

    void setUbuntuOneToken(UbuntuOne::Token token) { m_token = token; }
    UbuntuOne::Token getUbuntuOneToken() { return m_token; }

Q_SIGNALS:
    void updatesFound();
    void updatesNotFound();
    void errorOccurred();
    void networkError();
    void serverError();
    void clickTokenObtained(Update *app, const QString &clickToken);
    void credentialError();

private Q_SLOTS:
    void onUpdatesCheckFinished();
    void onReplySslErrors(const QList<QSslError> & errors);
    void onReplyError(QNetworkReply::NetworkError code);

private:
    QNetworkAccessManager m_nam;
    QNetworkConfigurationManager *m_ncm;
    QHash<QString, Update*> m_apps;
    UbuntuOne::Token m_token;

    QString getUrlApps();
    QString getFrameworksDir();
    bool replyIsValid(QNetworkReply *reply);
    void onTokenRequestFinished(Update* update, QNetworkReply* reply);

protected:
    virtual std::string architectureFromDpkg();
    virtual std::vector<std::string> listFolder(const std::string &folder, const std::string &pattern);

};

}

#endif // NETWORK_H
