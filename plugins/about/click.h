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

#ifndef CLICK_H
#define CLICK_H

#include <QObject>
#include <QProcess>
#include <QQmlListProperty>

class Click : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString clickName READ clickName NOTIFY clickUpdate)
    Q_PROPERTY( QString clickIcon READ clickIcon NOTIFY clickUpdate)
    Q_PROPERTY( QString clickInstall READ clickInstall NOTIFY clickUpdate)

public:
    explicit Click(const QString &name,
                   const QString &icon,
                   const QString &install,
                   QObject *parent = 0);
    ~Click();
    QString clickName() const;
    QString clickIcon() const;
    QString clickInstall() const;

Q_SIGNALS:
    void clickUpdate();

private:
    QString m_clickName;
    QString m_clickIcon;
    QString m_clickInstall;
};

#endif // CLICK_H
