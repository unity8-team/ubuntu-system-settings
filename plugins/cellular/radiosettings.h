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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/

#ifndef RADIOSETTINGS_H
#define RADIOSETTINGS_H

#include <QObject>
#include <QDBusReply>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QProcess>


class RadioSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY ( QString technologyPreference READ technologyPreference NOTIFY technologyPreferenceChanged )
    Q_ENUMS(TechnologyPreference)

public:
    explicit RadioSettings(QObject *parent = 0);
    // empty destructor
    ~RadioSettings();
    // string holding the current tech pref (any/gsm/umts/lte)

    void setPreference (QString pref);
    QString getPreference();
    QString technologyPreference();

    enum TechnologyPreference {
        UnknownPreference = 0,
        AnyPreference,
        GsmPreference,
        UmtsPreference,
        LtePreference
    };

signals:
    // signal for when this preference change
    void technologyPreferenceChanged(QString pref);

private:
    // list of possible preferences
    QList<QString*> m_technologyPreferences;

    // DBus constants
    const QString c_service;
    const QString c_object;

    // DBus members
    QDBusConnection m_systemBusConnection;
    QDBusServiceWatcher m_serviceWatcher;
    QDBusInterface m_radioSettingsInterface;

    void setUpInterface();

public Q_SLOTS:
    void slotChanged(QString, QVariantMap, QStringList);
    void slotNameOwnerChanged(QString, QString, QString);

Q_SIGNALS:
    void technologyPreferenceChanged();

};

#endif // RADIOSETTINGS_H
