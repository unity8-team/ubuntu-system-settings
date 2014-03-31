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

#ifndef FAKENETWORK_H
#define FAKENETWORK_H

#include <QObject>
#include <QHash>
#include <QString>
#include "update.h"

namespace UpdatePlugin {

class FakeNetwork : public QObject
{
    Q_OBJECT
public:
    explicit FakeNetwork(QObject *parent = 0);

    void checkForNewVersions(QHash<QString, Update*> &apps);
    void getResourceUrl(const QString& packagename);
    void getClickToken(Update* app, const QString& url, const QString& authHeader);
    
signals:
    void updatesFound();
    void updatesNotFound();
    void errorOccurred();
    void downloadUrlFound(const QString& packagename, const QString& url);
    void clickTokenObtained(Update* app, const QString& clickToken);
};

}

#endif // FAKENETWORK_H
