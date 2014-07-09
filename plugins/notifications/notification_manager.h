/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Diego Sarmentero <diego.sarmentero@canonical.com>
 *
*/

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QtQml>
#include <QList>
#include <QVariant>
#include <QVariantList>

namespace NotificationsPlugin {

class NotificationsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList model READ model NOTIFY modelChanged)

Q_SIGNALS:
    void modelChanged();

public:
    explicit NotificationsManager(QObject *parent = 0);
    ~NotificationsManager();

    QVariantList model() const { return m_model; }

private Q_SLOTS:
    void checkUpdates(QString id, bool value);

private:
    QVariantList m_model;

    void loadModel();
};

}

#endif // NOTIFICATIONMANAGER_H
