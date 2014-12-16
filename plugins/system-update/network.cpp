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

#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QProcessEnvironment>
#include <QUrl>

#include "network.h"

namespace {
    const QString CLICK_TOKEN_URL_ENV = "CLICK_TOKEN_URL";
    const QString URL_APPS_ENV = "URL_APPS";
    const QString URL_APPS =
        "https://myapps.developer.ubuntu.com/dev/api/click-metadata/";
    const QString APPS_DATA = "APPS_DATA";
    const QString NAME_KEY = "name";
    const QString VERSION_KEY = "version";
    const QString ICON_KEY = "icon_url";
    const QString DOWNLOAD_URL_KEY = "download_url";
    const QString BINARY_SIZE_KEY = "binary_filesize";
}

namespace UpdatePlugin {

Network::Network(QObject *parent) :
    QObject(parent),
    m_nam(this)
{
}

Network::Network(QHash<QString, Update*> apps, QObject *parent) :
    Network(parent)
{
    m_apps = apps;
}

void Network::checkForNewVersions(QHash<QString, Update*> &apps)
{
    m_apps = apps;

    QJsonObject serializer;
    QJsonArray array;

    foreach(QString id, m_apps.keys()) {
        array.append(QJsonValue(m_apps.value(id)->getPackageName()));
    }
    serializer.insert(NAME_KEY, array);

    QJsonDocument doc(serializer);

    auto content = doc.toJson();
    auto urlApps = getUrlApps();

    QNetworkRequest request(urlApps);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    auto reqObject = new RequestObject(APPS_DATA);
    request.setOriginatingObject(reqObject);

    auto reply = m_nam.post(request, content);
    connect(reply, &QNetworkReply::finished,
            this, &Network::onRequestFinished);
}

QUrl Network::getUrlApps()
{
    auto environment = QProcessEnvironment::systemEnvironment();
    auto command = environment.value(URL_APPS_ENV, URL_APPS);
    return command;
}

bool Network::parseUpdateObject(const QJsonValue& value)
{
    if (!value.isObject()) {
        qWarning() << "The value to parse is not an array.";
        return false;
    }

    auto object = value.toObject();

    if (object.contains(NAME_KEY)) {
        auto name = object[NAME_KEY].toString();

        if (name.isEmpty()) {
            qWarning() << "Json reply contains a '" << NAME_KEY
                << "' value that is not a string.";
            return false;
        }

        if (m_apps.contains(name)) {
            auto version = object[VERSION_KEY].toString();
            auto icon_url = object[ICON_KEY].toString();
            auto url = object[DOWNLOAD_URL_KEY].toString();
            auto size = object[BINARY_SIZE_KEY].toInt();

            if (!version.isEmpty()) {
                m_apps[name]->setRemoteVersion(version);
                if (m_apps[name]->updateRequired()) {
                    m_apps[name]->setIconUrl(icon_url);
                    m_apps[name]->setDownloadUrl(url);
                    m_apps[name]->setBinaryFilesize(size);
                    return true;
                }
            } else {
                qWarning() << "Json reply contains a '" << VERSION_KEY
                    << "' value that is not a string.";
                return false;
            }

        } else {
            // can we really get here? It means that an update was returned
            // for an application not present in the device
            qWarning() << "Reply has update for missing application";
            return false;
        }
    } else {
        qWarning() << "Json reply is missing the '" << NAME_KEY
            << "' key";
        return false;
    }
    return false;
}

void Network::onHeadRequestFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->hasRawHeader(X_CLICK_TOKEN)) {
        auto app = qobject_cast<Update*>(reply->request().originatingObject());
        if (app != nullptr) {
            QString header(reply->rawHeader(X_CLICK_TOKEN));
            Q_EMIT clickTokenObtained(app, header);
        } else {
            // is this even possible?
            qWarning() << "App missing from head request.";
        }
    } else {
        qWarning() << "HEader '" << X_CLICK_TOKEN << "' missing from head reply.";
    }
    reply->deleteLater();
}

void Network::onRequestFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        QVariant statusAttr = reply->attribute(
                                QNetworkRequest::HttpStatusCodeAttribute);
        if (!statusAttr.isValid()) {
            Q_EMIT errorOccurred();
            return;
        }

        int httpStatus = statusAttr.toInt();

        if (httpStatus == 200 || httpStatus == 201) {
            auto payload = reply->readAll();
            auto document = QJsonDocument::fromJson(payload);

            auto state = qobject_cast<RequestObject*>(
                    reply->request().originatingObject());

            if (state != nullptr
                    && state->operation.contains(APPS_DATA)
                    && document.isArray()) {

                auto array = document.array();
                bool updates = false;

                foreach(const QJsonValue& value, array) {
                    // parse the object and state if updates are needed
                    updates = parseUpdateObject(value);
                }

                if (updates) {
                    Q_EMIT updatesFound();
                } else {
                    Q_EMIT updatesNotFound();
                }

            } else {
                Q_EMIT errorOccurred();
            }
        } else {
            Q_EMIT errorOccurred();
        }
    } else if (reply->error() == QNetworkReply::TemporaryNetworkFailureError ||
               reply->error() == QNetworkReply::UnknownNetworkError) {
        Q_EMIT networkError();
    } else {
        Q_EMIT serverError();
    }

    reply->deleteLater();
}

void Network::getClickToken(Update *app, const QString &url,
                            const QString &authHeader)
{
    auto environment = QProcessEnvironment::systemEnvironment();
    auto signUrl = environment.value(CLICK_TOKEN_URL_ENV, url);

    QUrl query(signUrl);
    query.setQuery(authHeader);

    QNetworkRequest request(query);
    request.setOriginatingObject(app);

    auto reply = m_nam.head(request);
    connect(reply, &QNetworkReply::finished,
            this, &Network::onHeadRequestFinished);
}

}
