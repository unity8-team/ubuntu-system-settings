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

#ifndef FAKEDOWNLOADER_H
#define FAKEDOWNLOADER_H

#include <QObject>

namespace UpdatePlugin {

class FakeDownloader : public QObject
{
    Q_OBJECT

public:
    explicit FakeDownloader(QObject *parent = 0);
    ~FakeDownloader() {}
    
    void startDownload(QString packagename, QString url, const QString& authHeader);
    void setValid(bool value) { valid = value; }

signals:
    void downloadCreated(const QString& packagename, const QString& dbuspath);
    void downloadNotCreated(const QString& packagename, const QString& error);

private:
    bool valid;
    
};

}

#endif // FAKEDOWNLOADER_H
