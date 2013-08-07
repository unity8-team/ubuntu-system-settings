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

#ifndef NETWORKOPERATOR_H
#define NETWORKOPERATOR_H

#include <QObject>
#include <QtCore>
#include <ofononetworkoperator.h>

class NetworkOperator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_ENUMS(CellDataTechnology)

public:
    NetworkOperator(const QString& operatorId, QObject *parent=0);
    //NetworkOperator(const NetworkOperator& op);
    ~NetworkOperator();

    enum CellDataTechnology {
        UnknownDataTechnology = 0,
        GprsDataTechnology,
        EdgeDataTechnology,
        UmtsDataTechnology,
        HspaDataTechnology
    };

    /* Properties */
    QString name() const;
    QString status() const;

signals:
    void nameChanged(const QString &name);
    void statusChanged(const QString &status);

private:
    OfonoNetworkOperator *m_ofonoNetworkOperator;
    QString m_name;
    QString m_status;
    CellDataTechnology technologyToInt(const QString &technology);

private Q_SLOTS:
    void operatorNameChanged(const QString &name);
    void operatorStatusChanged(const QString &status);
};

#endif // NETWORKOPERATOR_H
