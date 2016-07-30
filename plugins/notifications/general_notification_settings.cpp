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
*/

#include <QtCore/QTimer>
#include <QtCore/QVariant>

#include "general_notification_settings.h"

#define GSETTINGS_APPS_SCHEMA_ID "com.ubuntu.notifications.settings.applications"
#define GSETTINGS_VIBRATE_SILENT_MODE_KEY "vibrate-silent-mode"

GeneralNotificationSettings::GeneralNotificationSettings(QObject* parent)
    : QObject(parent),
    m_settings(0),
    m_vibrateInSilentMode(true)
{
    QTimer::singleShot(0, this, SLOT(getSettings()));
}

GeneralNotificationSettings::~GeneralNotificationSettings()
{
}

void GeneralNotificationSettings::getSettings()
{
    m_settings.reset(new QGSettings(GSETTINGS_APPS_SCHEMA_ID));

    connect(m_settings.data(), SIGNAL(changed(const QString&)), SLOT(onSettingsChanged(const QString&)));

    m_vibrateInSilentMode = m_settings->get(GSETTINGS_VIBRATE_SILENT_MODE_KEY).toBool();
    Q_EMIT vibrateInSilentModeChanged();
}

void GeneralNotificationSettings::onSettingsChanged(const QString& key) {
    if (key == GSETTINGS_VIBRATE_SILENT_MODE_KEY) {
        bool vibrate = m_settings->get(GSETTINGS_VIBRATE_SILENT_MODE_KEY).toBool();
        if (vibrateInSilentMode() != vibrate) {
            m_vibrateInSilentMode = vibrate;
            Q_EMIT vibrateInSilentModeChanged();
        }
    }
}

bool GeneralNotificationSettings::vibrateInSilentMode()
{
    return m_vibrateInSilentMode;
}

void GeneralNotificationSettings::setVibrateInSilentMode(bool vibrate)
{
    if (vibrateInSilentMode() == vibrate) {
        return;
    }

    m_vibrateInSilentMode = vibrate;
    m_settings->set(GSETTINGS_VIBRATE_SILENT_MODE_KEY, m_vibrateInSilentMode);
    Q_EMIT vibrateInSilentModeChanged();
}
