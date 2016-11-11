/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SETTINGS_INTERFACE_H
#define SETTINGS_INTERFACE_H

//Qt
#include <QObject>

namespace qtmir
{

class SettingsInterface: public QObject
{
    Q_OBJECT
public:
    explicit SettingsInterface(QObject *parent = 0): QObject(parent) {}

    virtual QVariant get(const QString &key) const = 0;

Q_SIGNALS:
    void changed(const QString &key);
};

}

#endif // SETTINGS_INTERFACE_H
