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
#include <QObject>
#include <QtDebug>

#include "cellular.h"

Cellular::Cellular(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_radioSettingsIface ("org.ofono.RadioSettings",
                            "/org/ofono/RadioSettings/ril_0",
                            "org.ofono.RadioSettings",
                             m_systemBusConnection)
{
    qCritical() << "Cellular: ctor";
    if (!m_radioSettingsIface.isValid()) {
        qCritical() << "Cellular: interface invalid: " << m_radioSettingsIface.lastError();
        return;
    }
    qCritical() << "Cellular: valid";

    connect (&m_radioSettingsIface,
             SIGNAL (propertyChanged (QString, QString)),
             this,
             SLOT (slotChanged (QString, QString)));

    QDBusReply<QStringList> prefs = m_radioSettingsIface.call("GetProperties");
    if (!prefs.isValid()) {
        qCritical() << "Error:" << prefs.error().message();
        exit(1);
    }
    foreach (QString name, prefs.value())
        qCritical() << name;
}

QString Cellular::getTechnologyPreference()
{

    qCritical() << m_radioSettingsIface.call("GetProperties");
    return "any";
}

void Cellular::setTechnologyPreference(QString &pref)
{
    if (pref == getTechnologyPreference())
        return;

    Q_EMIT(technologyPreferenceChanged());
}

/* Converts the preference provided from ofono as a string to an enum.
 * The possible values from ofono are: "any", "gsm", "umts", "lte"
 */
Cellular::TechnologyPreference technologyPreferenceToInt(const QString &preference)
{
    if (preference == QString(QStringLiteral("any")))
        return Cellular::AnyTechnologyPreference;
    else if (preference == QString(QStringLiteral("gsm")))
        return Cellular::GsmTechnologyPreference;
    else if (preference == QString(QStringLiteral("umts")))
        return Cellular::UmtsTechnologyPreference;
    else if (preference == QString(QStringLiteral("lte")))
        return Cellular::LteTechnologyPreference;
    return Cellular::UnknownTechnologyPreference;
}

void Cellular::slotChanged(QString interface,
                                  QString property)
{
    qCritical() << "Interface: " << interface;
    qCritical() << "Property: " << property;
    if (property == "TechnologyPreference") {
        Q_EMIT technologyPreferenceChanged();
    }
}


/* Contains the enum of the technology of the current network.
 *
 * The possible values are:
 *     UnknownTechnologyPreference
 *     AnyTechnologyPreference
 *     GsmTechnologyPreference
 *     UmtsTechnologyPreference
 *     LteTechnologyPreference
 *
 */
Cellular::TechnologyPreference Cellular::technologyPreference() const
{
    return m_technologyPreference;
}
