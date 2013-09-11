/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#ifndef STORAGEABOUT_H
#define STORAGEABOUT_H

#include <QObject>
#include <QProcess>
#include <QVariant>

class StorageAbout : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString serialNumber
                READ serialNumber
                CONSTANT)

    Q_PROPERTY( QString vendorString
                READ vendorString
                CONSTANT)

    Q_PROPERTY( QString updateDate
                READ updateDate
                CONSTANT)

public:
    explicit StorageAbout(QObject *parent = 0);
    ~StorageAbout();
    Q_INVOKABLE QVariant buildClickView();
    Q_INVOKABLE QByteArray getClickList() const;
    Q_INVOKABLE QString getClickDir(const QString &name) const;
    QString serialNumber();
    QString vendorString();
    QString updateDate();
    Q_INVOKABLE QString licenseInfo(const QString &subdir) const;

private:
    QString m_serialNumber;
    QString m_vendorString;
    QString m_updateDate;
    QList<QObject*> m_clickList;
};

#endif // STORAGEABOUT_H
