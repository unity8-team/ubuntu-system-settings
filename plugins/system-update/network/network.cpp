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
#include <QScopedPointer>

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
}

std::string Network::getArchitecture()
{
    static const std::string deb_arch {architectureFromDpkg()};
    return deb_arch;
}

std::vector<std::string> Network::getAvailableFrameworks()
{
    std::vector<std::string> result;
    for (auto f: listFolder(getFrameworksDir().toStdString(), FRAMEWORKS_PATTERN)) {
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
    request.setRawHeader(QByteArray("X-Ubuntu-Frameworks"), QByteArray::fromStdString(frameworks.str()));
    request.setRawHeader(QByteArray("X-Ubuntu-Architecture"), QByteArray::fromStdString(getArchitecture()));
    request.setUrl(QUrl(urlApps));

    RequestObject* reqObject = new RequestObject(QString(APPS_DATA));
    request.setOriginatingObject(reqObject);

    auto reply = m_nam.post(request, content);

    connect(reply, &QNetworkReply::finished, this, &Network::onReplyFinished);
    connect(reply, &QNetworkReply::sslErrors, this, &Network::onReplySslErrors);
    connect(reply, static_cast<void(QNetworkReply::*)
            (QNetworkReply::NetworkError)>(&QNetworkReply::error),
        this, &Network::onReplyError);
}

QString Network::getUrlApps()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("URL_APPS", QString(URL_APPS));
    return command;
}

QString Network::getFrameworksDir()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("FRAMEWORKS_FOLDER", QString(FRAMEWORKS_FOLDER));
    return command;
}

void Network::onReplyFinished()
{
    // the scoped pointer will take care of calling the deleteLater when leaving the slot
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(qobject_cast<QNetworkReply*>(sender()));
    auto statusAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusAttr.isValid()) {
        Q_EMIT errorOccurred();
        return;
    }

    int httpStatus = statusAttr.toInt();

    if (httpStatus == 200 || httpStatus == 201) {
        if (reply->hasRawHeader(X_CLICK_TOKEN)) {
            auto app = qobject_cast<Update*>(reply->request().originatingObject());
            if (app != nullptr) {
                QString header(reply->rawHeader(X_CLICK_TOKEN));
                Q_EMIT clickTokenObtained(app, header);
            }
            return;
        }

        QByteArray payload = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(payload);

        auto state = qobject_cast<RequestObject*>(reply->request().originatingObject());
        if (state != nullptr && state->operation.contains(APPS_DATA) && document.isArray()) {
            QJsonArray array = document.array();
            bool updates = false;
            for (int i = 0; i < array.size(); i++) {
                auto object = array.at(i).toObject();
                auto name = object["name"].toString();
                auto version = object["version"].toString();
                auto icon_url = object["icon_url"].toString();
                auto url = object["download_url"].toString();
                auto download_sha512 = object["download_sha512"].toString();
                auto size = object["binary_filesize"].toInt();
                if (m_apps.contains(name)) {
                    m_apps[name]->setRemoteVersion(version);
                    if (m_apps[name]->updateRequired()) {
                        m_apps[name]->setIconUrl(icon_url);
                        m_apps[name]->setDownloadUrl(url);
                        m_apps[name]->setBinaryFilesize(size);
                        m_apps[name]->setDownloadSha512(download_sha512);
                        updates = true;
                    }
                }
            }
            if (updates) {
                Q_EMIT updatesFound();
                return;
            } else {
                Q_EMIT updatesNotFound();
                return;
            }
        }
    }

    Q_EMIT errorOccurred();
}

void Network::onReplySslErrors(const QList<QSslError>&)
{
    auto reply = sender();
    // Should this be a server or a network error??
    Q_EMIT serverError();
    reply->deleteLater();
}

void Network::onReplyError(QNetworkReply::NetworkError code)
{
    auto reply = sender();
    switch (code) {
        case QNetworkReply::TemporaryNetworkFailureError:
        case QNetworkReply::UnknownNetworkError:
        case QNetworkReply::UnknownProxyError:
        case QNetworkReply::UnknownServerError:
            Q_EMIT networkError();
            break;
        default:
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

    auto reply = m_nam.head(request);

    connect(reply, &QNetworkReply::finished, this, &Network::onReplyFinished);
    connect(reply, &QNetworkReply::sslErrors, this, &Network::onReplySslErrors);
    connect(reply, static_cast<void(QNetworkReply::*)
                    (QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &Network::onReplyError);
}

}
