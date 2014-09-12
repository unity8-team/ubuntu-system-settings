/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Diego Sarmentero <diego.sarmentero@canonical.com>
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

#include "update-plugin.h"

#include <QDebug>
#include <QStringList>
#include <SystemSettings/ItemBase>

#include <token.h>
#include <ssoservice.h>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QHash>
#include "../system_update.h"
#include "../update.h"
#include "../network/network.h"

using namespace SystemSettings;
using namespace UbuntuOne;
using namespace UpdatePlugin;

class UpdateItem: public ItemBase
{
    Q_OBJECT

public:
    explicit UpdateItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);
    ~UpdateItem();

Q_SIGNALS:
    void checkFinished();
    void modelChanged();
    void updatesNotFound();
    void credentialsNotFound();
    void updateAvailableFound(bool downloading);
    void errorFound();
    void downloadModeChanged();
    void systemUpdateDownloaded();
    void updateProcessFailed(QString message);
    void systemUpdateFailed(int consecutiveFailureCount, QString lastReason);

private Q_SLOTS:
    void changeVisibility(const QString&, Update*);
    void processOutput();
    void processUpdates();
    void handleCredentialsFound(Token token);

private:
    SystemUpdate m_systemUpdate;
    Token m_token;
    UpdatePlugin::Network m_network;
    QProcess m_process;
    SSOService m_service;
    QHash<QString, Update*> m_apps;
};

UpdateItem::UpdateItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent),
    m_systemUpdate(this)
{
    setVisibility(false);
    // SYSTEM UPDATE
    QObject::connect(&m_systemUpdate, SIGNAL(updateAvailable(const QString&, Update*)),
                  this, SLOT(changeVisibility(const QString&, Update*)));

    // SSO SERVICE
    QObject::connect(&m_service, SIGNAL(credentialsFound(const Token&)),
                     this, SLOT(handleCredentialsFound(Token)));
    // PROCESS
    QObject::connect(&m_process, SIGNAL(finished(int)),
                  this, SLOT(processOutput()));
    // NETWORK
    QObject::connect(&m_network, SIGNAL(updatesFound()),
                  this, SLOT(processUpdates()));

    m_systemUpdate.checkForUpdate();
    m_service.getCredentials();
}

void UpdateItem::handleCredentialsFound(Token token)
{
    m_token = token;
    QStringList args("list");
    args << "--manifest";
    QString command("click");
    m_process.start(command, args);
}

void UpdateItem::processOutput()
{
    QString output(m_process.readAllStandardOutput());

    QJsonDocument document = QJsonDocument::fromJson(output.toUtf8());

    QJsonArray array = document.array();

    int i;
    for (i = 0; i < array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        QString name = object.value("name").toString();
        QString title = object.value("title").toString();
        QString version = object.value("version").toString();
        Update *app = new Update();
        app->initializeApplication(name, title, version);
        m_apps[app->getPackageName()] = app;
    }

    m_network.checkForNewVersions(m_apps);
}


void UpdateItem::processUpdates()
{
    foreach (QString id, m_apps.keys()) {
        Update *app = m_apps.value(id);
        if(app->updateRequired()) {
            setVisibility(true);
            break;
        }
    }
}

void UpdateItem::changeVisibility(const QString&, Update*)
{
    setVisibility(true);
}

void UpdateItem::setVisibility(bool visible)
{
    setVisible(visible);
}

UpdateItem::~UpdateItem()
{
}

CheckUpdatesPlugin::CheckUpdatesPlugin():
    QObject()
{
}


ItemBase *CheckUpdatesPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new UpdateItem(staticData, parent);
}

#include "update-plugin.moc"
