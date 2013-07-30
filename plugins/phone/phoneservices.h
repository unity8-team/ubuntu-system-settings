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

#ifndef PHONESERVICES_H
#define PHONESERVICES_H

#include <QObject>
#include <QtCore>
#include <ofonosimmanager.h>

class PhoneServices : public QObject
{
    Q_OBJECT
    Q_PROPERTY ( QVariant serviceNumbers READ serviceNumbers NOTIFY serviceNumbersChanged )


public:
    explicit PhoneServices(QObject *parent = 0);
    ~PhoneServices();
    QVariant serviceNumbers();

signals:
    void serviceNumbersChanged();

private:
    QList<QObject*> m_serviceNumbers;
    OfonoServiceNumbers m_ofonoServiceNumbers;
    OfonoSimManager *m_ofonoSimManager;
    void populateServiceNumbers (OfonoServiceNumbers);

private Q_SLOTS:
    void simServiceNumbersChanged(OfonoServiceNumbers sn);

};

#endif // PHONESERVICES_H
