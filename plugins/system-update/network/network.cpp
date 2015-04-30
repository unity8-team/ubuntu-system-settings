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
#include <sstream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QUrl>
#include <QProcessEnvironment>

namespace {
    const QString URL_APPS = "https://search.apps.ubuntu.com/api/v1/click-metadata";
    const QString APPS_DATA = "APPS_DATA";
    constexpr static const char* FRAMEWORKS_FOLDER {"/usr/share/click/frameworks/"};
    constexpr static const char* FRAMEWORKS_PATTERN {"*.framework"};
    constexpr static const int FRAMEWORKS_EXTENSION_LENGTH = 10; // strlen(".framework")
}

namespace UpdatePlugin {

Network::Network(QObject *parent) :
    QObject(parent),
    m_nam(this)
{
    QObject::connect(&m_nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(onReply(QNetworkReply*)));
}

std::string Network::getArchitecture()
{
    static const std::string deb_arch {architectureFromDpkg()};
    return deb_arch;
}

std::vector<std::string> Network::getAvailableFrameworks()
{
    std::vector<std::string> result;
    for (auto f: listFolder(FRAMEWORKS_FOLDER, FRAMEWORKS_PATTERN)) {
        result.push_back(f.substr(0, f.size()-FRAMEWORKS_EXTENSION_LENGTH));
    }
    return result;
}

std::string Network::architectureFromDpkg()
{
    QString program("dpkg");
    QStringList arguments;
    arguments << "--print-architecture";
    QProcess archDetector;
    archDetector.start(program, arguments);
    if(!archDetector.waitForFinished()) {
        qWarning() << "Architecture detection failed.";
    }
    auto output = archDetector.readAllStandardOutput();
    auto ostr = QString::fromUtf8(output);

    return ostr.trimmed().toStdString();
}

std::vector<std::string> Network::listFolder(const std::string& folder, const std::string& pattern)
{
    std::vector<std::string> result;

    QDir dir(QString::fromStdString(folder), QString::fromStdString(pattern),
                                    QDir::Unsorted, QDir::Readable | QDir::Files);
    QStringList entries = dir.entryList();
    for (int i = 0; i < entries.size(); ++i) {
        QString filename = entries.at(i);
        result.push_back(filename.toStdString());
    }

    return result;
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
    std::stringstream frameworks;
    for (auto f: getAvailableFrameworks()) {
        frameworks << "," << f;
    }
    QJsonDocument doc(serializer);
    QByteArray content = doc.toJson();

    QString urlApps = getUrlApps();
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QByteArray("X-Ubuntu-Frameworks"), QByteArray(frameworks.str().c_str()));
    request.setRawHeader(QByteArray("X-Ubuntu-Architecture"), QByteArray(get_architecture().c_str()));
    request.setUrl(QUrl(urlApps));
    RequestObject* reqObject = new RequestObject(QString(APPS_DATA));
    request.setOriginatingObject(reqObject);
    m_nam.post(request, content);
}

QString Network::getUrlApps()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("URL_APPS", QString(URL_APPS));
    return command;
}

void Network::onReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
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
                if (app != nullptr) {
                    QString header(reply->rawHeader(X_CLICK_TOKEN));
                    Q_EMIT clickTokenObtained(app, header);
                }
                reply->deleteLater();
                return;
            }

            QByteArray payload = reply->readAll();
            QJsonDocument document = QJsonDocument::fromJson(payload);

            RequestObject* state = qobject_cast<RequestObject*>(reply->request().originatingObject());
            if (state != nullptr && state->operation.contains(APPS_DATA) && document.isArray()) {
                QJsonArray array = document.array();
                bool updates = false;
                for (int i = 0; i < array.size(); i++) {
                    QJsonObject object = array.at(i).toObject();
                    QString name = object.value("name").toString();
                    QString version = object.value("version").toString();
                    QString icon_url = object.value("icon_url").toString();
                    QString url = object.value("download_url").toString();
                    int size = object.value("binary_filesize").toVariant().toInt();
                    if (m_apps.contains(name)) {
                        m_apps[name]->setRemoteVersion(version);
                        if (m_apps[name]->updateRequired()) {
                            m_apps[name]->setIconUrl(icon_url);
                            m_apps[name]->setDownloadUrl(url);
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
