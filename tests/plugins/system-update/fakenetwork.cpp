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

#include "fakenetwork.h"

namespace UpdatePlugin {

FakeNetwork::FakeNetwork(QObject *parent) :
    QObject(parent)
{
}

void FakeNetwork::checkForNewVersions(QHash<QString, Update*> &apps)
{
    if(apps.contains("com.ubuntu.developer.xda-app")) {
        Update* app = apps.value("com.ubuntu.developer.xda-app");
        QString version("0.5.2ubuntu2");
        app->setRemoteVersion(version);
        emit this->updatesFound();
    }
}

void FakeNetwork::getResourceUrl(const QString& packagename)
{
    emit this->downloadUrlFound(packagename, "http://canonical.com");
}

void FakeNetwork::getClickToken(Update* app, const QString& url, const QString& authHeader)
{
    Q_UNUSED(url);
    Q_UNUSED(authHeader);
    QString fakeHeader("x-click-token-header");
    emit this->clickTokenObtained(app, fakeHeader);
}

}
