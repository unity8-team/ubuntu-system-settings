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

#include "network.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QUrl>
#include <QProcessEnvironment>

#define URL_APPS "https://myapps.developer.ubuntu.com/dev/api/click-metadata/"
#define URL_PACKAGE "https://search.apps.ubuntu.com/api/v1/package/"

namespace UpdatePlugin {

Network::Network(QObject *parent) :
    QObject(parent),
    m_nam(this)
{
    m_request.setHeader(QNetworkRequest::ContentTypeHeader,
                              "application/json");

    QObject::connect(&m_nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(onReply(QNetworkReply*)));
}

void Network::checkForNewVersions(QHash<QString, Update*> &apps)
{
    m_apps = apps;

    QJsonObject serializer;
    QJsonArray array;
    foreach(QString id, m_apps.keys()) {
        array.append(QJsonValue(m_apps.value(id)->getPackageName()));
    }

    serializer.insert("name", array);
    QJsonDocument doc(serializer);

    QByteArray content = doc.toJson();

    QString urlApps = getUrlApps();
    m_request.setUrl(QUrl(urlApps));
    m_nam.post(m_request, content);
}



QString Network::getUrlApps()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("URL_APPS", QString(URL_APPS));
    return command;
}

QString Network::getUrlPackage()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("URL_PACKAGE", QString(URL_PACKAGE));
    return command;
}

void Network::onReply(QNetworkReply *reply)
{
    QVariant statusAttr = reply->attribute(
                            QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusAttr.isValid()) {
        Q_EMIT errorOccurred();
        return;
    }

    int httpStatus = statusAttr.toInt();

    if (httpStatus == 200 || httpStatus == 201) {
        if (reply->hasRawHeader(X_CLICK_TOKEN)) {
            Update* app = qobject_cast<Update*>(
                        reply->request().originatingObject());
            if (app != NULL) {
                QString header(reply->rawHeader(X_CLICK_TOKEN));
                Q_EMIT clickTokenObtained(app, header);
            }
            reply->deleteLater();
            return;
        }

        QByteArray payload = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(payload);

        if (document.isArray()) {
            QJsonArray array = document.array();
            int i;
            bool updates = false;
            for (i = 0; i < array.size(); i++) {
                QJsonObject object = array.at(i).toObject();
                QString name = object.value("name").toString();
                QString version = object.value("version").toString();
                QString icon_url = object.value("icon_url").toString();
                int size = object.value("binary_filesize").toVariant().toInt();
                if (m_apps.contains(name)) {
                    m_apps[name]->setRemoteVersion(version);
                    if (m_apps[name]->updateRequired()) {
                        m_apps[name]->setIconUrl(icon_url);
                        m_apps[name]->setBinaryFilesize(size);
                        updates = true;
                    }
                }
            }
            if (updates) {
                Q_EMIT updatesFound();
            } else {
                Q_EMIT updatesNotFound();
            }
        } else if (document.isObject()) {
            QJsonObject object = document.object();
            QString url = object.value("download_url").toString();
            QString name = object.value("name").toString();
            Q_EMIT downloadUrlFound(name, url);
        } else {
            Q_EMIT errorOccurred();
        }
    } else {
        Q_EMIT errorOccurred();
    }

    reply->deleteLater();
}

void Network::getResourceUrl(const QString &packagename)
{
    QString urlPackage = getUrlPackage();
    m_request.setUrl(QUrl(urlPackage + packagename));
    m_nam.get(m_request);
}

void Network::getClickToken(Update *app, const QString &url,
                            const QString &authHeader)
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString signUrl = environment.value("CLICK_TOKEN_URL", url);
    QUrl query(signUrl);
    query.setQuery(authHeader);
    QNetworkRequest request;
    request.setUrl(query);
    request.setOriginatingObject(app);
    m_nam.head(request);
}

}
