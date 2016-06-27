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

#include "clickupdatemanager.h"
#include "helpers.h"

#include <assert.h>

#include <QByteArray>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>


namespace UpdatePlugin
{
ClickUpdateManager::ClickUpdateManager(QObject *parent)
        : QObject(parent)
        , m_db(SystemUpdate::instance()->updateDb())
        , m_process()
        , m_apiClient(this)
        , m_updates()
        , m_authToken(UbuntuOne::Token())
        , m_authenticated(true)
        , m_checking(false)
{
    init();
}

ClickUpdateManager::ClickUpdateManager(const QString &dbpath, QObject *parent)
        : QObject(parent)
        , m_db(new UpdateDb(dbpath, this))
        , m_process()
        , m_apiClient(this)
        , m_updates()
        , m_authToken(UbuntuOne::Token())
        , m_authenticated(true)
        , m_checking(false)
{
    init();
}

void ClickUpdateManager::init()
{
    initializeProcess();
    initializeSSOService();
    initializeApiClient();

    connect(this, SIGNAL(checkStarted()), this, SLOT(handleCheckStart()));
    connect(this, SIGNAL(checkCompleted()), this, SLOT(handleCheckStop()));
    connect(this, SIGNAL(checkCompleted()), this, SLOT(handleCheckCompleted()));
    connect(this, SIGNAL(checkFailed()), this, SLOT(handleCheckStop()));
    connect(this, SIGNAL(checkCanceled()), this, SLOT(handleCheckStop()));
}

ClickUpdateManager::~ClickUpdateManager()
{
}

void ClickUpdateManager::initializeTokenDownloader(const ClickTokenDownloader *dler)
{
    connect(dler, SIGNAL(tokenRequestSucceeded(Update*)),
            this, SLOT(handleTokenDownload(Update*)));
    connect(dler, SIGNAL(tokenRequestFailed(Update*)),
            this, SLOT(handleTokenDownloadFailure(Update*)));
    connect(this, SIGNAL(checkCanceled()),
            dler, SLOT(cancel()));
}

void ClickUpdateManager::initializeProcess()
{
    connect(&m_process, SIGNAL(finished(const int&)),
            this, SLOT(processInstalledClicks(const int&)));
}

void ClickUpdateManager::initializeSSOService()
{
    connect(&m_ssoService, SIGNAL(credentialsFound(const Token&)),
            this, SLOT(handleCredentialsFound(const Token&)));
    connect(&m_ssoService, SIGNAL(credentialsNotFound()),
            this, SLOT(handleCredentialsFailed()));
    connect(&m_ssoService, SIGNAL(credentialsDeleted()),
            this, SLOT(handleCredentialsFailed()));
}

void ClickUpdateManager::initializeApiClient()
{
    connect(&m_apiClient, SIGNAL(success(QNetworkReply*)),
            this, SLOT(handleMetadata(QNetworkReply*)));
    connect(&m_apiClient, SIGNAL(serverError()),
            this, SLOT(handleCommunicationErrors()));
    connect(&m_apiClient, SIGNAL(networkError()),
            this, SLOT(handleCommunicationErrors()));

    connect(&m_apiClient, SIGNAL(networkError()),
            this, SIGNAL(networkError()));
    connect(&m_apiClient, SIGNAL(serverError()),
            this, SIGNAL(serverError()));
    connect(&m_apiClient, SIGNAL(credentialError()),
            this, SIGNAL(credentialError()));
}

// Basically, there are three async operations, with one for each
// updated click (fetching a click token). Each of these can fail in various
// ways, so we report either serverError or networkError with an accompanying
// errorString.
//
// The first operation is obtaining the list of clicks installed on the system.
// The second is receiving the update data for clicks we found installed.
// The third is signing the download url for each click update. We ask the
// ClickUpdate to do this for us.
//
// All of this can be bypassed if we find cached update data newer than 24 hours.
void ClickUpdateManager::check()
{
    qWarning() << "click checker: check...";

    // Don't check for click updates if there are no credentials,
    // instead ask for credentials.
    if (!m_authToken.isValid() && !Helpers::isIgnoringCredentials()) {
        m_ssoService.getCredentials();
        return;
    }

    Q_EMIT (checkStarted());

    m_updates.clear();
    m_apiClient.setErrorString("");

    // Start process of getting the list of installed clicks.
    QStringList args("list");
    args << "--manifest";
    QString command = Helpers::whichClick();
    m_process.start(command, args);
    if (!m_process.waitForStarted()) {
        handleProcessError(m_process.error());
    }
}

void ClickUpdateManager::check(const QString &packageName)
{
    // qWarning() << "click checker: checking this one file" << packageName
    //         << "...";
    // ClickUpdate *m = m_db->getPendingClickUpdate(packageName);
    // qWarning() << "click um: got back" << m;
    // if (m && m->identifier() == packageName) {
    //     qWarning() << "click checker: requesting click token for" << packageName
    //             << "...";
    //     initializeTokenDownloader(m);
    //     m->setAutomatic(true);
    //     m->requestClickToken();
    // }
}

// Tries to shut down all checks we might currently be doing.
void ClickUpdateManager::cancel()
{
    // Abort all click update update data objects.
    // foreach (const QString &name, m_updates.keys())m_updates.value(name)->cancel();



    m_process.terminate();

    m_apiClient.abortNetworking();
    Q_EMIT checkCanceled();
}

void ClickUpdateManager::processInstalledClicks(const int &exitCode)
{
    qWarning() << "click checker: processInstalledClicks..." << exitCode;

    if (exitCode == 15) {
        // 15 is terminated in pythonese, i.e. normal if cancel was called.
        Q_EMIT checkCanceled();
        return;
    }

    if (exitCode > 0) {
        QString e("Failed to enumerate installed clicks (%1).");
        m_apiClient.setErrorString(e.arg(exitCode));
        Q_EMIT checkFailed();
        return;
    }

    QString output(m_process.readAllStandardOutput());
    QJsonDocument document = QJsonDocument::fromJson(output.toUtf8());
    QJsonArray array = document.array();

    // Nothing to do.
    if (array.size() == 0) {
        qWarning() << "click checker: nothing to do, no clicks installed.";
        Q_EMIT checkCompleted();
        return;
    }

    int i;
    for (i = 0; i < array.size(); i++) {
        Update *update = new Update(this);

        QJsonObject object = array.at(i).toObject();
        update->setIdentifier(object.value("name").toString());
        update->setTitle(object.value("title").toString());
        update->setLocalVersion(object.value("version").toString());

        QStringList command;
        // command << Helpers::whichPkcon() << "-p" << "install-local" << "$file";
        command << "sleep" << "5";
        update->setCommand(command);

        m_updates.insert(update->identifier(), update);
        qWarning() << "click checker: queueing up" << update->identifier();
    }

    // Populate each ClickUpdate with remote update data.
    requestClickMetadata();
}

void ClickUpdateManager::handleProcessError(const QProcess::ProcessError &error)
{
    qWarning() << "click checker: process failed";
    switch (error) {
    case QProcess::FailedToStart:
        qWarning() << "QProcess::FailedToStart";
        break;
    case QProcess::Crashed:
        qWarning() << "QProcess::Crashed";
        break;
    case QProcess::Timedout:
        qWarning() << "QProcess::Timedout";
        break;
    case QProcess::WriteError:
        qWarning() << "QProcess::WriteError";
        break;
    case QProcess::ReadError:
        qWarning() << "QProcess::ReadError";
        break;
    case QProcess::UnknownError:
        qWarning() << "QProcess::UnknownError";
        break;
    }
    Q_EMIT checkFailed();
}

void ClickUpdateManager::handleCheckCompleted()
{
    m_db->setLastCheckDate(QDateTime::currentDateTime());
}

void ClickUpdateManager::handleTokenDownload(Update *update)
{
    qWarning() << "click checker: handling obtained token on update data"
            << update->identifier();

    // m_db->add(update);

    completionCheck();
}

void ClickUpdateManager::completionCheck()
{
    qWarning() << "click checker: checking for completion...";
    qWarning() << "click checker: completion check had"
            << m_updates.keys().count() << "keys";

    // Check if all tokens are fetched.
    foreach (const QString &name, m_updates.keys()){
        if (m_updates.value(name)->token().isEmpty()) {
            qWarning() << "click checker: not complete.";
            return; // Not done.
        }
    }

    // All updates had signed download urls, so we're done.
    Q_EMIT checkCompleted();
    qWarning() << "click checker: complete.";
}

void ClickUpdateManager::handleTokenDownloadFailure(Update *update)
{
    // Unset token, let the user try again.
    update->setToken("");
    // m_db->add(update);

    // We're done with it.
    m_updates.remove(update->identifier());
    completionCheck();
}

void ClickUpdateManager::handleCredentialsFound(const UbuntuOne::Token &token)
{
    qWarning() << "found credentials";
    m_authToken = token;

    if (!m_authToken.isValid()) {
        qWarning() << "updateManager got invalid token.";
        handleCredentialsFailed();
        return;
    }

    setAuthenticated(true);

    cancel();
    check();
}

void ClickUpdateManager::handleCredentialsFailed()
{
    qWarning() << "failed credentials";
    m_ssoService.invalidateCredentials();
    m_authToken = UbuntuOne::Token();

    cancel();

    // We've invalidated the token, and the user is now not authenticated.
    setAuthenticated(false);
}

void ClickUpdateManager::handleCommunicationErrors()
{
    if (m_checking)
        Q_EMIT (checkFailed());
}

void ClickUpdateManager::requestClickMetadata()
{
    qWarning() << "click checker: asking for remote update data...";
    QJsonObject serializer;

    // Construct the “name” list
    QJsonArray array;
    foreach (const QString &name, m_updates.keys())array.append(QJsonValue(name));
    serializer.insert("name", array);

    // Create list of frameworks.
    std::stringstream frameworks;
    for (auto f : Helpers::getAvailableFrameworks()) {
        frameworks << "," << f;
    }

    QJsonDocument doc(serializer);
    QByteArray content = doc.toJson();

    QString urlApps = Helpers::clickMetadataUrl();
    qWarning() << "click checker: using url" << urlApps;
    QString authHeader = m_authToken.signUrl(
        urlApps, QStringLiteral("POST"), true
    );
    QUrl url(urlApps);
    url.setQuery(authHeader);

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QByteArray("X-Ubuntu-Frameworks"),
            QByteArray::fromStdString(frameworks.str()));
    request.setRawHeader(QByteArray("X-Ubuntu-Architecture"),
            QByteArray::fromStdString(Helpers::getArchitecture()));
    request.setUrl(url);

    QNetworkReply *reply = m_apiClient.nam()->post(request, content);
    m_apiClient.initializeReply(reply);
}

void ClickUpdateManager::handleMetadata(QNetworkReply *reply)
{
    qWarning() << "click checker: request succeeded...";

    QJsonParseError *jsonError = new QJsonParseError;
    auto document = QJsonDocument::fromJson(reply->readAll(), jsonError);

    if (document.isArray()) {
        parseClickMetadata(document.array());
    } else {
        m_apiClient.setErrorString("Got unexpected JSON from server.");
        handleCommunicationErrors();
    }

    if (jsonError->error != QJsonParseError::NoError) {
        m_apiClient.setErrorString("json parse failed: " + jsonError->errorString());
        handleCommunicationErrors();
    }

    delete jsonError;
    reply->deleteLater();
}

void ClickUpdateManager::parseClickMetadata(const QJsonArray &array)
{
    for (int i = 0; i < array.size(); i++) {
        auto object = array.at(i).toObject();
        auto name = object["name"].toString();

        qWarning() << "click checker: got update data for" << name;

        auto version = object["version"].toString();
        auto icon_url = object["icon_url"].toString();
        auto url = object["download_url"].toString();
        auto download_sha512 = object["download_sha512"].toString();
        auto changelog = object["changelog"].toString();
        auto size = object["binary_filesize"].toInt();
        auto title = object["title"].toString();
        auto revision = object["revision"].toInt();
        if (m_updates.contains(name)) {
            Update *update = m_updates.value(name);
            update->setRemoteVersion(version);
            if (update->isUpdateRequired()) {

                qWarning() << "click checker: update of" << update->identifier()
                        << "is required";

                update->setIconUrl(icon_url);
                update->setDownloadUrl(url);
                update->setBinaryFilesize(size);
                update->setDownloadHash(download_sha512);
                update->setChangelog(changelog);
                update->setTitle(title);
                update->setRevision(revision);

                // Start the process of obtaining a click token for this
                // click update.
                ClickTokenDownloader* dl = new ClickTokenDownloader(this, update);
                dl->setAuthToken(m_authToken);
                initializeTokenDownloader(dl);
                dl->requestToken();
            } else {
                // Update not required, let's remove it.
                m_updates.remove(update->identifier());
                completionCheck();
            }
        }
    }

    // Prune m_updates, removing those without necessary update data. These are
    // either locally installed clicks, or retracted from the API (?).
    qWarning() << "click checker: pruning...";
    foreach (const QString &name, m_updates.keys()) {
        qWarning() << name << m_updates.value(name)->remoteVersion();
        if (m_updates.value(name)->remoteVersion().isEmpty())
            m_updates.remove(name);
    }
    completionCheck();
}

bool ClickUpdateManager::isCheckRequired()
{
    // Spec says that a manual check should not happen if a check was
    // completed less than 30 minutes ago.
    QDateTime now = QDateTime::currentDateTimeUtc().addSecs(-1800); // 30 mins
    return m_db->lastCheckDate() < now;
}

bool ClickUpdateManager::authenticated()
{
    return m_authenticated || Helpers::isIgnoringCredentials();
}

void ClickUpdateManager::setAuthenticated(const bool authenticated)
{
    if (authenticated != m_authenticated) {
        m_authenticated = authenticated;
        Q_EMIT (authenticatedChanged());
    }
}
} // UpdatePlugin
