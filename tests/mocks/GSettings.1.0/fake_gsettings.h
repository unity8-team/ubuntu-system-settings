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

#ifndef FAKE_GSETTINGS_H
#define FAKE_GSETTINGS_H

#include <QList>
#include <QObject>
#include <QQmlParserStatus>
#include <QStringList>
#include <QVariant>

class GSettingsSchemaQml: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QByteArray id READ id WRITE setId)

public:
    GSettingsSchemaQml(QObject *parent = nullptr);

    QByteArray id() const;
    void setId(const QByteArray &id);

    QByteArray path() const;
    void setPath(const QByteArray &path);

private:
    QByteArray m_id;
    QByteArray m_path;
};

class GSettingsQml: public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(GSettingsSchemaQml* schema READ schema NOTIFY schemaChanged)
    Q_PROPERTY(QVariant autoBrightness READ autoBrightness WRITE setAutoBrightness NOTIFY autoBrightnessChanged)
    Q_PROPERTY(QVariant launcherWidth READ launcherWidth WRITE setLauncherWidth NOTIFY launcherWidthChanged)
    Q_PROPERTY(QVariant autohideLauncher READ autohideLauncher WRITE setAutohideLauncher NOTIFY autohideLauncherChanged)

public:
    GSettingsQml(QObject *parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

    GSettingsSchemaQml * schema() const;
    QVariant autoBrightness() const;
    QVariant launcherWidth() const;
    QVariant autohideLauncher() const;

    void setAutoBrightness(const QVariant &val);
    void setLauncherWidth(const QVariant &val);
    void setAutohideLauncher(const QVariant &val);

Q_SIGNALS:
    void autoBrightnessChanged();
    void launcherWidthChanged();
    void autohideLauncherChanged();
    void schemaChanged();

private:
    GSettingsSchemaQml* m_schema;
    bool m_valid;

    friend class GSettingsSchemaQml;
};

class GSettingsControllerQml: public QObject
{
    Q_OBJECT

public:
    static GSettingsControllerQml* instance();
    ~GSettingsControllerQml();

    bool autoBrightness() const;
    uint launcherWidth() const;
    bool autohideLauncher() const;
    
    Q_INVOKABLE void setAutoBrightness(bool val);
    Q_INVOKABLE void setLauncherWidth(uint val);
    Q_INVOKABLE void setAutohideLauncher(bool val);

Q_SIGNALS:
    void autoBrightnessChanged();
    void launcherWidthChanged();
    void autohideLauncherChanged();

private:
    GSettingsControllerQml();

    bool m_autoBrightness;
    uint m_launcherWidth = 8;
    bool m_autohideLauncher = true;

    static GSettingsControllerQml* s_controllerInstance;
    QList<GSettingsQml *> m_registeredGSettings;
};

#endif // FAKE_GSETTINGS_H
