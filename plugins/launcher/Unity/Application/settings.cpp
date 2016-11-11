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

// Ubuntu
#include <QGSettings>
#include <QVariant>

// local
#include "settings.h"

namespace qtmir
{

Settings::Settings(QObject *parent)
    :SettingsInterface(parent)
    ,m_settings(new QGSettings("com.canonical.qtmir", "/com/canonical/qtmir/"))
{
    connect(m_settings, &QGSettings::changed, this, &Settings::changed);
}

QVariant Settings::get(const QString &key) const
{
    return m_settings->get(key);
}

} // namespace qtmir
