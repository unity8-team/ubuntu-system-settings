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

#include <assert.h>

#include <QByteArray>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>

#include "clickupdatemanager.h"
#include "helpers.h"

namespace UpdatePlugin
{
ClickUpdateManager::ClickUpdateManager(QObject *parent)
        : ClickApiClient(parent)
        , m_model(new UpdateModel(this))
        , m_process()
        , m_metas()
        , m_token(UbuntuOne::Token())
        , m_authenticated(true)
        , m_checking(false)
{
    init();
}

ClickUpdateManager::ClickUpdateManager(const QString &dbpath, QObject *parent)
        : ClickApiClient(parent)
        , m_model(new UpdateModel(dbpath, this))
        , m_process()
        , m_metas()
        , m_token(UbuntuOne::Token())
        , m_authenticated(true)
        , m_checking(false)
{
    init();
}

void ClickUpdateManager::init()
{
    initializeProcess();
    initializeSSOService();

    connect(this, SIGNAL(serverError()), this, SLOT(handleCommunicationErrors()));
    connect(this, SIGNAL(networkError()), this, SLOT(handleCommunicationErrors()));

    connect(this, SIGNAL(checkStarted()), this, SLOT(handleCheckStart()));
    connect(this, SIGNAL(checkCompleted()), this, SLOT(handleCheckStop()));
    connect(this, SIGNAL(checkCompleted()), this, SLOT(handleCheckCompleted()));
    connect(this, SIGNAL(checkFailed()), this, SLOT(handleCheckStop()));
    connect(this, SIGNAL(checkCanceled()), this, SLOT(handleCheckStop()));
}

ClickUpdateManager::~ClickUpdateManager()
{
}

void ClickUpdateManager::initializeMeta(const ClickUpdateMetadata *meta)
{
    QObject::connect(meta, SIGNAL(credentialError()), this,
            SIGNAL(credentialError()));
    QObject::connect(meta, SIGNAL(credentialError()), this,
            SLOT(handleCredentialsFailed()));
    QObject::connect(meta,
            SIGNAL(clickTokenRequestSucceeded(const ClickUpdateMetadata*)),
            this, SLOT(processClickToken(const ClickUpdateMetadata*)));
    QObject::connect(meta,
            SIGNAL(clickTokenRequestFailed(ClickUpdateMetadata*)), this,
            SLOT(handleClickTokenFailure(ClickUpdateMetadata*)));
}

void ClickUpdateManager::initializeProcess()
{
    QObject::connect(&m_process, SIGNAL(finished(const int&)), this,
            SLOT(processInstalledClicks(const int&)));
}

void ClickUpdateManager::initializeSSOService()
{
    connect(&m_ssoService, SIGNAL(credentialsFound(const Token&)), this,
            SLOT(handleCredentialsFound(const Token&)));
    connect(&m_ssoService, SIGNAL(credentialsNotFound()), this,
            SLOT(handleCredentialsFailed()));
    connect(&m_ssoService, SIGNAL(credentialsDeleted()), this,
            SLOT(handleCredentialsFailed()));
}

// Basically, there are three async operations, with one for each
// updated click (fetching a click token). Each of these can fail in various
// ways, so we report either serverError or networkError with an accompanying
// errorString.
//
// The first operation is obtaining the list of clicks installed on the system.
// The second is receiving the metadata for clicks we found installed.
// The third is signing the download url for each click update. We ask the
// ClickUpdateMetadata to do this for us.
//
// All of this can be bypassed if we find cached metadata newer than 24 hours.
void ClickUpdateManager::check()
{
    qWarning() << "click checker: check...";

    // Don't check for click updates if there are no credentials,
    // instead ask for credentials.
    if (!m_token.isValid() && !Helpers::isIgnoringCredentials()) {
        m_ssoService.getCredentials();
        return;
    }

    Q_EMIT (checkStarted());

    m_metas.clear();
    setErrorString("");

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
    qWarning() << "click checker: checking this one file" << packageName
            << "...";
    ClickUpdateMetadata *m = m_model->getPending(packageName);
    if (m->name() == packageName) {
        qWarning() << "click checker: requesting click token for" << packageName
                << "...";
        initializeMeta(m);
        m->setAutomatic(true);
        m->requestClickToken();
    }
}

// Tries to shut down all checks we might currently be doing.
void ClickUpdateManager::cancel()
{
    // Abort all click update metadata objects.
    foreach (const QString &name, m_metas.keys())m_metas.value(name)->cancel();

    m_process.terminate();

    // This tells active replies to abort.
    Q_EMIT abortNetworking();
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
        setErrorString(e.arg(exitCode));
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
        ClickUpdateMetadata *meta = new ClickUpdateMetadata(this);
        meta->setToken(m_token);
        initializeMeta(meta);

        QJsonObject object = array.at(i).toObject();
        meta->setName(object.value("name").toString());
        meta->setTitle(object.value("title").toString());
        meta->setLocalVersion(object.value("version").toString());

        QStringList command;
        // command << Helpers::whichPkcon() << "-p" << "install-local" << "$file";
        command << "sleep" << "5";
        meta->setCommand(command);

        m_metas.insert(meta->name(), meta);
        qWarning() << "click checker: queueing up" << meta->name();
    }

    // Populate each ClickUpdateMetadata with remote metadata.
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
    m_model->setLastCheckDate(QDateTime::currentDateTime());
}

void ClickUpdateManager::processClickToken(const ClickUpdateMetadata *meta)
{
    qWarning() << "click checker: handling obtained token on metadata"
            << meta->name();

    m_model->add(meta);

    completionCheck();
}

void ClickUpdateManager::completionCheck()
{
    qWarning() << "click checker: checking for completion...";
    qWarning() << "click checker: completion check had"
            << m_metas.keys().count() << "keys";

    // Check if all tokens are fetched.
    foreach (const QString &name, m_metas.keys()){
        if (m_metas.value(name)->clickToken().isEmpty()) {
            qWarning() << "click checker: not complete.";
            return; // Not done.
        }
    }

    // All metas had signed download urls, so we're done.
    Q_EMIT checkCompleted();
    qWarning() << "click checker: complete.";
}

void ClickUpdateManager::handleClickTokenFailure(
        ClickUpdateMetadata *meta)
{
    // Set empty click token in db. This way we can ask the user to retry.
    meta->setClickToken("");
    m_model->add(meta);

    // We're done with it.
    m_metas.remove(meta->name());
    completionCheck();
}


void ClickUpdateManager::handleCredentialsFound(const UbuntuOne::Token &token)
{
    qWarning() << "found credentials";
    m_token = token;

    if (!m_token.isValid()) {
        qWarning() << "updateManager got invalid token.";
        handleCredentialsFailed();
        return;
    }

    setAuthenticated(true);

    // Set click update checker's token, cancel and start a new check.
    setToken(token);
    cancel();
    check();
}

void ClickUpdateManager::handleCredentialsFailed()
{
    qWarning() << "failed credentials";
    m_ssoService.invalidateCredentials();
    m_token = UbuntuOne::Token();

    // Ask click update checker to stop checking for updates.
    // Revoke the token given to click update checker.
    cancel();
    setToken(m_token);

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
    qWarning() << "click checker: asking for remote metadata...";
    QJsonObject serializer;

    // Construct the “name” list
    QJsonArray array;
    foreach (const QString &name, m_metas.keys())array.append(QJsonValue(name));
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
    QString authHeader = m_token.signUrl(urlApps, QStringLiteral("POST"), true);
    QUrl url(urlApps);
    url.setQuery(authHeader);

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QByteArray("X-Ubuntu-Frameworks"),
            QByteArray::fromStdString(frameworks.str()));
    request.setRawHeader(QByteArray("X-Ubuntu-Architecture"),
            QByteArray::fromStdString(Helpers::getArchitecture()));
    request.setUrl(url);

    QNetworkReply *reply = m_nam.post(request, content);
    initializeReply(reply);
}

void ClickUpdateManager::requestSucceeded(QNetworkReply *reply)
{
    qWarning() << "click checker: request succeeded...";

    QJsonParseError *jsonError = new QJsonParseError;
    auto document = QJsonDocument::fromJson(reply->readAll(), jsonError);

    if (document.isArray()) {
        parseClickMetadata(document.array());
    } else {
        setErrorString("Got unexpected JSON from server.");
        Q_EMIT serverError();
    }

    if (jsonError->error != QJsonParseError::NoError) {
        setErrorString("json parse failed: " + jsonError->errorString());
        Q_EMIT serverError();
    }

    delete jsonError;
    reply->deleteLater();
}

void ClickUpdateManager::parseClickMetadata(const QJsonArray &array)
{
    for (int i = 0; i < array.size(); i++) {
        auto object = array.at(i).toObject();
        auto name = object["name"].toString();

        qWarning() << "click checker: got metadata for" << name;

        auto version = object["version"].toString();
        auto icon_url = object["icon_url"].toString();
        auto url = object["download_url"].toString();
        auto download_sha512 = object["download_sha512"].toString();
        auto changelog = object["changelog"].toString();
        auto size = object["binary_filesize"].toInt();
        auto title = object["title"].toString();
        auto revision = object["revision"].toInt();
        if (m_metas.contains(name)) {
            ClickUpdateMetadata *meta = m_metas.value(name);
            meta->setRemoteVersion(version);
            if (meta->isUpdateRequired()) {

                qWarning() << "click checker: update of" << meta->name()
                        << "is required";

                meta->setIconUrl(icon_url);
                meta->setDownloadUrl(url);
                meta->setBinaryFilesize(size);
                meta->setDownloadSha512(download_sha512);
                meta->setChangelog(changelog);
                meta->setTitle(title);
                meta->setRevision(revision);

                // Start the process of obtaining a click token for this
                // click update.
                meta->requestClickToken();
            } else {
                // Update not required, let's remove it.
                m_metas.remove(meta->name());
                completionCheck();
            }
        }
    }

    // Prune m_metas, removing those without necessary metadata. These are
    // either locally installed clicks, or retracted from the API (?).
    qWarning() << "click checker: pruning...";
    foreach (const QString &name, m_metas.keys()) {
        qWarning() << name << m_metas.value(name)->remoteVersion();
        if (m_metas.value(name)->remoteVersion().isEmpty())
            m_metas.remove(name);
    }
    completionCheck();
}

bool ClickUpdateManager::isCheckRequired()
{
    // Spec says that a manual check should not happen if a check was
    // completed less than 30 minutes ago.
    QDateTime now = QDateTime::currentDateTimeUtc().addSecs(-1800); // 30 mins
    return m_model->lastCheckDate() < now;
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

// void ClickUpdateManager::markInstalled(const QString &packageName, const int &revision)
// {
//     m_model->markInstalled(packageName, revision);
// }


// void ClickUpdateManager::setUpdateState(const QString &packageName, const int &revision,
//                                         const int &state)
// {
//     SystemUpdate::UpdateState u = (SystemUpdate::UpdateState) state;
//     m_model->setUpdateState(packageName, revision, u);
// }

// void ClickUpdateManager::setProgress(const QString &packageName, const int &revision,
//                                      const int &progress)
// {
//     m_model->setProgress(packageName, revision, progress);
// }
} // UpdatePlugin
