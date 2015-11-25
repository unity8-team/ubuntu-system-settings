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
    constexpr static const char* FRAMEWORKS_FOLDER {"/usr/share/click/frameworks/"};
    constexpr static const char* FRAMEWORKS_PATTERN {"*.framework"};
    constexpr static const int FRAMEWORKS_EXTENSION_LENGTH = 10; // strlen(".framework")
}

namespace UpdatePlugin {

Network::Network(QObject *parent) :
    QObject(parent),
    m_nam(this),
    m_ncm(new QNetworkConfigurationManager()),
    m_reply(0)
{
    qWarning() << __PRETTY_FUNCTION__;
}

Network::~Network()
{
    qWarning() << __PRETTY_FUNCTION__;
    if (m_reply) {
        m_reply->abort();
        delete m_reply;
    }
}

std::string Network::getArchitecture()
{
    qWarning() << __PRETTY_FUNCTION__;
    static const std::string deb_arch {architectureFromDpkg()};
    return deb_arch;
}

std::vector<std::string> Network::getAvailableFrameworks()
{
    qWarning() << __PRETTY_FUNCTION__;
    std::vector<std::string> result;
    for (auto f: listFolder(getFrameworksDir().toStdString(), FRAMEWORKS_PATTERN)) {
        result.push_back(f.substr(0, f.size()-FRAMEWORKS_EXTENSION_LENGTH));
    }
    return result;
}

bool Network::replyIsValid(QNetworkReply *reply)
{
    qWarning() << __PRETTY_FUNCTION__;
    auto statusAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusAttr.isValid()) {
        Q_EMIT errorOccurred();
        return false;
    }

    int httpStatus = statusAttr.toInt();
    qWarning() << "HTTP Status: " << httpStatus;

    if (httpStatus == 401 || httpStatus == 403) {
        qWarning() << "Emitting credetials error.";
        Q_EMIT credentialError();
        return false;
    }

    return true;
}

void Network::onTokenRequestFinished(Update* app, QNetworkReply* r)
{
    qWarning() << __PRETTY_FUNCTION__;
    // the scoped pointer will take care of calling the deleteLater when leaving the slot
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(r);

    // check if the reply is valid, this method will emit all the required signals to propagate errors to the rest
    // of the plugin
    if (!replyIsValid(r)) {
        qWarning() << "Reply is not valid.";
        return;
    }

    if (reply->hasRawHeader(X_CLICK_TOKEN)) {
        if (app != nullptr) {
            QString header(reply->rawHeader(X_CLICK_TOKEN));
            Q_EMIT clickTokenObtained(app, header);
        }
        return;
    }

    Q_EMIT errorOccurred();
}

std::string Network::architectureFromDpkg()
{
    qWarning() << __PRETTY_FUNCTION__;
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
    qWarning() << __PRETTY_FUNCTION__;
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
    qWarning() << __PRETTY_FUNCTION__;

    // If we aren't online, don't check
    if (!m_ncm->isOnline()) {
        qWarning() << "Not currently online, don't check";
        return;
    }

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
    QString authHeader = m_token.signUrl(urlApps, QStringLiteral("POST"), true);
    QUrl url(urlApps);
    url.setQuery(authHeader);

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QByteArray("X-Ubuntu-Frameworks"), QByteArray::fromStdString(frameworks.str()));
    request.setRawHeader(QByteArray("X-Ubuntu-Architecture"), QByteArray::fromStdString(getArchitecture()));
    request.setUrl(url);

    m_reply = m_nam.post(request, content);

    connect(m_reply, &QNetworkReply::finished, this, &Network::onUpdatesCheckFinished);
    connect(m_reply, &QNetworkReply::sslErrors, this, &Network::onReplySslErrors);
    connect(m_reply, static_cast<void(QNetworkReply::*)
            (QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &Network::onReplyError);
    connect(m_ncm, &QNetworkConfigurationManager::onlineStateChanged, [&](const bool &online) {
            if (!online && m_reply) {
                qWarning() << "Offline, aborting check for updates";
                qWarning() << Q_FUNC_INFO << "isRunning:" << m_reply->isRunning();
                if (m_reply->isRunning())
                    m_reply->abort();
                m_reply = 0;
            }
    });
}



QString Network::getUrlApps()
{
    qWarning() << __PRETTY_FUNCTION__;
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("URL_APPS", QString(URL_APPS));
    return command;
}

QString Network::getFrameworksDir()
{
    qWarning() << __PRETTY_FUNCTION__;
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString command = environment.value("FRAMEWORKS_FOLDER", QString(FRAMEWORKS_FOLDER));
    return command;
}

void Network::onUpdatesCheckFinished()
{
    qWarning() << __PRETTY_FUNCTION__;
    // the scoped pointer will take care of calling the deleteLater when leaving the slot
    auto r = qobject_cast<QNetworkReply*>(sender());

    // check for http error status and emit all the required signals
    if (!replyIsValid(r)) {
        qWarning() << "Reply is not valid.";
        return;
    }

    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(r);

    auto document = QJsonDocument::fromJson(reply->readAll());

    if (document.isArray()) {
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

    Q_EMIT errorOccurred();
}

void Network::onReplySslErrors(const QList<QSslError>&)
{
    qWarning() << __PRETTY_FUNCTION__;
    auto reply = sender();
    // Should this be a server or a network error??
    Q_EMIT serverError();
    reply->deleteLater();
    m_reply = 0;
}

void Network::onReplyError(QNetworkReply::NetworkError code)
{
    qWarning() << __PRETTY_FUNCTION__;
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
    m_reply = 0;
}

void Network::getClickToken(Update *app, const QString &url)
{
    qWarning() << __PRETTY_FUNCTION__;
    if (!m_token.isValid()) {
        app->setError("Invalid User Token");
        return;
    }

    QString authHeader = m_token.signUrl(app->downloadUrl(), QStringLiteral("HEAD"), true);
    app->setClickUrl(app->downloadUrl());

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString signUrl = environment.value("CLICK_TOKEN_URL", url);
    QUrl query(signUrl);
    query.setQuery(authHeader);
    QNetworkRequest request;
    request.setUrl(query);

    auto reply = m_nam.head(request);

    connect(reply, &QNetworkReply::finished, this, [=](){
        auto reply = qobject_cast<QNetworkReply*>(sender());
        onTokenRequestFinished(app, reply);
    });

    connect(reply, &QNetworkReply::sslErrors, this, &Network::onReplySslErrors);
    connect(reply, static_cast<void(QNetworkReply::*)
                    (QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &Network::onReplyError);
}

}
