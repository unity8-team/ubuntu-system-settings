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

#ifndef SIMSERVICE_H
#define SIMSERVICE_H

#include <QObject>

class SimService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)

public:
    SimService(QObject *parent=0);
    SimService(const QString &name, const QString &value, QObject *parent=0);

    QString name() const;
    void setName(const QString &name);

    QString value() const;
    void setValue(const QString &value);

signals:
    void nameChanged();
    void valueChanged();

private:
    QString m_name;
    QString m_value;
};

#endif // SIMSERVICE_H
