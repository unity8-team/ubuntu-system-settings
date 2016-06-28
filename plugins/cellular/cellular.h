/*
 * Copyright (C) 2016 Canonical Ltd
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

#ifndef CELLULAR_H
#define CELLULAR_H

#include "accountsservice.h"

#include <QObject>
#include <QVariantMap>

class Cellular : public QObject
{
    Q_OBJECT

public:
    explicit Cellular(QObject *parent = 0);
    Q_PROPERTY (QString defaultSimForCalls
                READ getDefaultSimForCalls
                WRITE setDefaultSimForCalls
                NOTIFY defaultSimForCallsChanged)
    Q_PROPERTY (QString defaultSimForMessages
                READ getDefaultSimForMessages
                WRITE setDefaultSimForMessages
                NOTIFY defaultSimForMessagesChanged)
    Q_PROPERTY (QVariantMap simNames
                READ getSimNames
                WRITE setSimNames
                NOTIFY simNamesChanged)

public Q_SLOTS:
    void slotChanged(QString, QString);
    void slotNameOwnerChanged();

Q_SIGNALS:
    void defaultSimForCallsChanged();
    void defaultSimForMessagesChanged();
    void simNamesChanged();

private:
    AccountsService m_accountsService;

    QString getDefaultSimForCalls();
    void setDefaultSimForCalls(QString sim);
    QString getDefaultSimForMessages();
    void setDefaultSimForMessages(QString sim);
    QVariantMap getSimNames();
    void setSimNames(QVariantMap names);

};

#endif // CELLULAR_H
