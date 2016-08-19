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

#ifndef NETWORK_ACCESS_MANAGER_H
#define NETWORK_ACCESS_MANAGER_H

#include <QObject>
#include <QByteArray>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace UpdatePlugin
{
namespace Network
{
class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~Manager() {};
    virtual QNetworkReply* post(const QNetworkRequest &request, const QByteArray &data) = 0;
    virtual QNetworkReply* head(const QNetworkRequest &request) = 0;
Q_SIGNALS:
    void finished(QNetworkReply *);
    void sslErrors(QNetworkReply *, const QList<QSslError>&);
};
} // Network
} // UpdatePlugin

#endif // NETWORK_ACCESS_MANAGER_H
