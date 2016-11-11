/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fake_gsettings.h"

#include <QDebug>
#include <QList>

GSettingsControllerQml* GSettingsControllerQml::s_controllerInstance = 0;

GSettingsControllerQml::GSettingsControllerQml()
    : m_autoBrightness(false)
{
}

GSettingsControllerQml::~GSettingsControllerQml() {
    s_controllerInstance = 0;
}

GSettingsControllerQml* GSettingsControllerQml::instance()  {
    if (!s_controllerInstance) {
        s_controllerInstance = new GSettingsControllerQml();
    }
    return s_controllerInstance;
}

bool GSettingsControllerQml::autoBrightness() const
{
    return m_autoBrightness;
}

void GSettingsControllerQml::setAutoBrightness(bool val)
{
    if (val != m_autoBrightness) {
        m_autoBrightness = val;
        Q_EMIT autoBrightnessChanged();
    }
}

uint GSettingsControllerQml::launcherWidth() const
{
    return m_launcherWidth;
}

bool GSettingsControllerQml::autohideLauncher() const
{
    return m_autohideLauncher;
}

void GSettingsControllerQml::setLauncherWidth(uint val)
{
    if (val != m_launcherWidth) {
        m_launcherWidth = val;
        Q_EMIT launcherWidthChanged();
    }
}

void GSettingsControllerQml::setAutohideLauncher(bool val)
{
    if (val != m_autohideLauncher) {
        m_autohideLauncher = val;
        Q_EMIT autohideLauncherChanged();
    }
}

GSettingsSchemaQml::GSettingsSchemaQml(QObject *parent): QObject(parent) {
}

QByteArray GSettingsSchemaQml::id() const {
    return m_id;
}

void GSettingsSchemaQml::setId(const QByteArray &id) {
    if (!m_id.isEmpty()) {
        qWarning("GSettings.schema.id may only be set on construction");
        return;
    }

    m_id = id;
}

QByteArray GSettingsSchemaQml::path() const {
    return m_path;
}

void GSettingsSchemaQml::setPath(const QByteArray &path) {
    if (!m_path.isEmpty()) {
        qWarning("GSettings.schema.path may only be set on construction");
        return;
    }

    m_path = path;
}

GSettingsQml::GSettingsQml(QObject *parent)
    : QObject(parent),
      m_valid(false)
{
    m_schema = new GSettingsSchemaQml(this);
}

void GSettingsQml::classBegin()
{
}

void GSettingsQml::componentComplete()
{
    // Emulate what the real GSettings module does, and only return undefined
    // values until we are completed loading.
    m_valid = true;

    // FIXME: We should make this dynamic, instead of hard-coding all possible
    // properties in one object.  We should create properties based on the schema.
    connect(GSettingsControllerQml::instance(), &GSettingsControllerQml::autoBrightnessChanged,
            this, &GSettingsQml::autoBrightnessChanged);
    connect(GSettingsControllerQml::instance(), &GSettingsControllerQml::autohideLauncherChanged,
            this, &GSettingsQml::autohideLauncherChanged);
    connect(GSettingsControllerQml::instance(), &GSettingsControllerQml::launcherWidthChanged,
            this, &GSettingsQml::launcherWidthChanged);

    Q_EMIT autoBrightnessChanged();
    Q_EMIT autohideLauncherChanged();
    Q_EMIT launcherWidthChanged();
}

GSettingsSchemaQml * GSettingsQml::schema() const {
    return m_schema;
}

QVariant GSettingsQml::autoBrightness() const
{
    if (m_valid && m_schema->id() == "com.ubuntu.touch.system") {
        return GSettingsControllerQml::instance()->autoBrightness();
    } else {
        return QVariant();
    }
}

void GSettingsQml::setAutoBrightness(const QVariant &val)
{
    if (m_valid && m_schema->id() == "com.ubuntu.touch.system") {
        GSettingsControllerQml::instance()->setAutoBrightness(val.toBool());
    }
}


QVariant GSettingsQml::launcherWidth() const
{
    if (m_valid && m_schema->id() == "com.canonical.Unity8") {
        return GSettingsControllerQml::instance()->launcherWidth();
    } else {
        return QVariant();
    }
}

QVariant GSettingsQml::autohideLauncher() const
{
    qWarning() << "autohideLauncher" << m_valid << m_schema->id() << GSettingsControllerQml::instance()->autohideLauncher();
    if (m_valid && m_schema->id() == "com.canonical.Unity8") {
        return GSettingsControllerQml::instance()->autohideLauncher();
    } else {
        return QVariant();
    }
}


void GSettingsQml::setLauncherWidth(const QVariant &val)
{
    if (m_valid && m_schema->id() == "com.canonical.Unity8") {
        GSettingsControllerQml::instance()->setLauncherWidth(val.toUInt());
    }
}

void GSettingsQml::setAutohideLauncher(const QVariant &val)
{
    if (m_valid && m_schema->id() == "com.canonical.Unity8") {
        GSettingsControllerQml::instance()->setAutohideLauncher(val.toBool());
    }
}
