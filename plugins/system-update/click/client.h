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

#ifndef CLICK_CLIENT_H
#define CLICK_CLIENT_H

#include <QObject>
#include <QList>
#include <QString>
#include <QUrl>
#include <QByteArray>

namespace UpdatePlugin
{
namespace Click
{
class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0) : QObject(parent) {};
    virtual ~Client() {};

    virtual void cancel() = 0;
    virtual void requestMetadata(const QUrl &url,
                                 const QList<QString> &packages) = 0;
    virtual void requestToken(const QUrl &url) = 0;

signals:
    void metadataRequestSucceeded(const QByteArray &metadata);
    void tokenRequestSucceeded(const QString &token);
    void networkError();
    void serverError();
    void credentialError();
    void abortNetworking();
};

} // Click
} // UpdatePlugin

#endif // CLICK_CLIENT_H
