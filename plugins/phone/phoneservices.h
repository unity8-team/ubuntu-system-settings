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
 * Ken VanDine <ken.vandine@canonical.com>
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
    Q_PROPERTY ( bool present READ present NOTIFY presenceChanged )

public:
    explicit PhoneServices(QObject *parent = 0);
    ~PhoneServices();

    /* Properties */
    QVariant serviceNumbers();
    bool present() const;

signals:
    void serviceNumbersChanged();
    void presenceChanged(bool ispresent);

private:
    QList<QObject*> m_serviceNumbers;
    OfonoServiceNumbers m_ofonoServiceNumbers;
    OfonoSimManager *m_ofonoSimManager;
    bool m_present;
    void populateServiceNumbers(OfonoServiceNumbers);

private Q_SLOTS:
    void simServiceNumbersChanged(OfonoServiceNumbers sn);
    void simPresenceChanged(bool ispresent);
};

#endif // PHONESERVICES_H
