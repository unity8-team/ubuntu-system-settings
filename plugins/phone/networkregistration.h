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

#ifndef NETWORKREGISTRATION_H
#define NETWORKREGISTRATION_H

#include <QObject>
#include <QtCore>
#include <ofononetworkregistration.h>

class NetworkRegistration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(int technology READ technology NOTIFY technologyChanged)
    Q_PROPERTY(QVariant operators READ operators NOTIFY operatorsChanged)
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)

    Q_ENUMS(CellDataTechnology)

public:
    explicit NetworkRegistration(QObject *parent = 0);
    ~NetworkRegistration();

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
    QVariant operators() const;
    CellDataTechnology technology() const;
    bool scanning() const;

public slots:
    void registerOp();
    void getOperators();
    void scan();

signals:
    void nameChanged(const QString &name);
    void statusChanged(const QString &status);
    void technologyChanged(const CellDataTechnology &technology);
    void operatorsChanged();
    void scanningChanged();

private:
    OfonoNetworkRegistration *m_ofonoNetworkRegistration;
    QString m_name;
    QString m_status;
    QList<QObject*> m_operators;
    CellDataTechnology m_technology;
    CellDataTechnology technologyToInt(const QString &technology);
    bool m_scanning;
    void populateOperators(QStringList);


private Q_SLOTS:
    void operatorNameChanged(const QString &name);
    void operatorStatusChanged(const QString &status);
    void operatorTechnologyChanged(const QString &technology);
    void operatorsUpdated(bool success, const QStringList &oplist);


};

#endif // NETWORKREGISTRATION_H
