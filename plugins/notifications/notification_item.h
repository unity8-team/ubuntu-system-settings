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

#ifndef UPDATE_H
#define UPDATE_H

#include <QObject>
#include <QtQml>
#include <QString>
#include <QStringList>

namespace NotificationsPlugin {

class NotificationItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(bool status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString key MEMBER m_key)

Q_SIGNALS:
    void statusChanged();
    void titleChanged();
    void iconChanged();
    void updateNotificationStatus(QString id, bool value);

public:
    explicit NotificationItem(QObject *parent = 0);
    virtual ~NotificationItem();
    void setItemData(QString title, QString icon, bool status, QString key);

    bool status() { return m_status; }
    QString title() { return m_title; }
    QString icon() { return m_icon; }

    void setStatus(bool status);

private:
    QString m_title;
    QString m_icon;
    QString m_key;
    bool m_status;
};

}

#endif // UPDATE_H
