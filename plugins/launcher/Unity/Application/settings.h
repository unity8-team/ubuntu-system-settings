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

#ifndef SETTINGS_H
#define SETTINGS_H

//Qt
#include <QObject>

// local
#include "settings_interface.h"

class QGSettings;

namespace qtmir
{

class Settings: public SettingsInterface
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);

    QVariant get(const QString &key) const override;

private:
    QGSettings *m_settings;
};

}

#endif // SETTINGS_H
